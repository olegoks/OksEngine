#pragma once 

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.CodeFile.hpp>
#include <ECSGenerator.SystemsGraph.hpp>


#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  


namespace ECSGenerator {

	class ParsedComponentECSFile : public ParsedECSFile {
	public:

		struct FieldInfo {
			Type type_;
			std::string typeName_;
			std::string name_;
			bool copyable_ = true;

			const std::string& GetName() const {
				return name_;
			}
			const std::string& GetTypeName() const {
				return typeName_;
			}
		};

		struct CreateInfo {
			std::string name_;
			std::filesystem::path path_;
			std::vector<FieldInfo> fields_;
		};

		ParsedComponentECSFile(const CreateInfo& createInfo)
			: ParsedECSFile{ createInfo.path_, createInfo.name_ },
			fields_{ createInfo.fields_ } { }


		std::vector<FieldInfo> fields_;

		bool AreThereFields() const {
			return !fields_.empty();
		}

		using ProcessField = std::function<bool(const FieldInfo& fieldInfo, bool isLast)>;

		void ForEachField(ProcessField&& processField) const {
			for (Common::Index i = 0; i < fields_.size(); i++) {
				const FieldInfo info = fields_[i];
				if (!processField(info, (i == fields_.size() - 1))) {
					break;
				}
			}
		}

		bool CanBeCreatedFromImGui() const {
			bool canCreateFromImgui = true;
			ForEachField([&canCreateFromImgui](const FieldInfo& fieldInfo, bool isLast) {
				if (!IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
					canCreateFromImgui = false;
					return false;
				}
				return true;
				});
			return canCreateFromImgui;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		Type GetType() const noexcept override {
			return Type::Component;
		}

	};


	inline std::shared_ptr<ParsedComponentECSFile> ParseComponentEcsFile(const std::filesystem::path& path, ::Lua::Context& context) {

		luabridge::LuaRef component = context.GetGlobalAsRef("component");
		luabridge::LuaRef fields = component["fields"];

		std::vector<ParsedComponentECSFile::FieldInfo> parsedFields;

		if (!fields.isNil()) {
			for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
				luabridge::LuaRef field = it.value();
				ParsedComponentECSFile::FieldInfo fieldInfo{
					.typeName_ = field["type"].cast<std::string>().value(),
					.name_ = field["name"].cast<std::string>().value()
				};

				if (!field["copyable"].isNil()) {
					fieldInfo.copyable_ = field["copyable"].cast<bool>().value();
				}

				parsedFields.push_back(fieldInfo);
			}
		}
		ParsedComponentECSFile::CreateInfo ci{
			.name_ = component["name"].cast<std::string>().value(),
			.path_ = path,
			.fields_ = parsedFields
		};
		auto parsedComponentFile = std::make_shared<ParsedComponentECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
		return parsedComponentFile;
	}


	//static char {variableName}[256] = "";
	//static Common::UInt64 {variableName};
	Code GenerateTypeImGuiInputVariable(const std::string& typeName, std::string variableName) {
		Code code;
		const std::string& fieldTypeName = typeName;
		if (fieldTypeName == "std::string") {
			code.Add("static char " + variableName + "[256] = \"\";");
			return code;
		}
		else if (
			fieldTypeName == "float" || 
			fieldTypeName == "double" || 
			fieldTypeName == "Common::Size" ||
			fieldTypeName == "Common::UInt64" || 
			fieldTypeName == "Common::Index" || 
			fieldTypeName == "ECS::Entity::Id") {
			code.Add("static " + typeName + " " + variableName + ";");
			return code;
		}
		OS::NotImplemented();
		return {};
	}

	//// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
	//// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
	Code GenerateImGuiInputTypeCode(const std::string& imguiVariableName, const std::string& typeName,  std::string outVariable) {
		Code code;
		if (typeName == "std::string") {
			code.Add("ImGui::InputText(\"" + imguiVariableName + "\", " + outVariable + ", IM_ARRAYSIZE(" + outVariable + "));");
			return code;
		}
		else if (
			typeName == "float" ||
			typeName == "double" ||
			typeName == "Common::Size" ||
			typeName == "Common::UInt64" ||
			typeName == "Common::Index" ||
			typeName == "ECS::Entity::Id") {
			code.Add("ImGui::InputScalar(\"" + imguiVariableName + "\", " + GetImGuiType(typeName) + ", " + outVariable + ");");
			return code;
		}
		OS::NotImplemented();
		return code;
	}

	Code GenerateTypeImGuiEditCode(
		const std::string& componentVariableName,			// position3D
		const std::string& fieldVariableTypeName,			// float
		const std::string& fieldVariableName,				// x
		const std::string& fieldComponentVariableName) {	// x_
		Code code;
		if (fieldVariableTypeName == "std::string") {
			code.Add(GenerateTypeImGuiInputVariable(fieldVariableTypeName, fieldVariableName));
			code.NewLine();
			code.Add("std::memcpy(" + fieldVariableName + ", " + componentVariableName + "->" + fieldComponentVariableName + ".c_str(), " + componentVariableName + "->" + fieldVariableName + "_.size());");
			code.NewLine();
			code.Add(GenerateImGuiInputTypeCode(fieldVariableTypeName, fieldVariableTypeName, fieldVariableName));
			code.NewLine();
			code.Add(componentVariableName + "->" + fieldComponentVariableName + " = std::string{ " + fieldVariableName + " };");
			code.NewLine();
			return code;
		}
		else if (
			fieldVariableTypeName == "float" || 
			fieldVariableTypeName == "double" || 
			fieldVariableTypeName == "Common::Size" || 
			fieldVariableTypeName == "Common::UInt64" || 
			fieldVariableTypeName == "Common::Index" || 
			fieldVariableTypeName == "ECS::Entity::Id") {
			code.Add(GenerateImGuiInputTypeCode(
				fieldVariableName,
				fieldVariableTypeName,
				"&" + componentVariableName + "->" + fieldComponentVariableName));
			return code;
		}

		OS::NotImplemented();

		return {};
	}



	class ComponentFileStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		ComponentFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}



		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedComponentECSFile> componentEcsFile) {

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			{

				std::vector<Struct::Field> fields;

				componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast)->bool {

					Struct::Field field{
						.type_ = fieldInfo.GetTypeName(),
						.name_ = fieldInfo.GetName(),
						.copyable_ = fieldInfo.copyable_
					};

					fields.push_back(field);

					return true;
					});
				//
				//				// ������� �������� � ������
				//				llvm::LLVMContext Context;
				//				llvm::Module* Mod = new llvm::Module("main_module", Context);
				//
				//				// ������� ������� main
				//				llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
				//				llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", Mod);
				//
				//				// ������� ������� ����
				//				llvm::BasicBlock* BB = llvm::BasicBlock::Create(Context, "entry", F);
				//				llvm::IRBuilder<> Builder(BB);
				//
				//				// ���������� 42
				//				Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 42));
				//
				//				// ��������� ������������ ������
				//				verifyFunction(*F);
				//
				//				// ������� LLVM IR
				//				//Mod->print(outs(), nullptr);
				//
				//				/*[[nodiscard]]
				//				static const char* GetName() noexcept {
				//				return Common::TypeInfo<Type>::GetTypeName();
				//				}*/
				//
				//				{
				//					// ������� ��������� �����������
				//					clang::CompilerInstance CI;
				//					CI.createDiagnostics();
				//#pragma region Assert
				//					OS::AssertMessage(CI.hasDiagnostics(), "");
				//#pragma endregion
				//					// ����������� LangOptions (��������� �����)
				//					clang::LangOptions LO;
				//					CI.getLangOpts() = LO;
				//					// ����������� ������� ������� (target triple)
				//					std::shared_ptr<clang::TargetOptions> TO = std::make_shared<clang::TargetOptions>();
				//					//TO->Triple = "x86_64-pc-windows-msvc";
				//					TO->Triple = llvm::sys::getDefaultTargetTriple(); // �������� ������� �� ���������
				//					CI.setTarget(clang::TargetInfo::CreateTargetInfo(CI.getDiagnostics(), TO));
				//					if (!CI.hasTarget()) {
				//						//llvm::errs() << "������: �� ������� ������� TargetInfo\n";
				//						//return 1;
				//					}
				//					// ����������� FileManager
				//					CI.createFileManager();
				//#pragma region Assert
				//					OS::AssertMessage(CI.hasFileManager(), "");
				//#pragma endregion
				//					// ����������� SourceManager
				//					CI.createSourceManager(CI.getFileManager());
				//#pragma region Assert
				//					OS::AssertMessage(CI.hasSourceManager(), "");
				//#pragma endregion
				//					// ����������� Preprocessor
				//					CI.createPreprocessor(clang::TranslationUnitKind::TU_Complete);
				//#pragma region Assert
				//					OS::AssertMessage(CI.hasPreprocessor(), "������: �� ������� ������� Preprocessor.");
				//#pragma endregion
				//					// ������� ASTContext
				//					CI.createASTContext();
				//
				//					// ������� Rewriter ��� ��������� ����
				//					clang::Rewriter R;
				//					R.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
				//
				//					// ������� FunctionDecl ��� main()
				//					clang::FunctionDecl* FD = clang::FunctionDecl::Create(
				//						CI.getASTContext(),
				//						CI.getASTContext().getTranslationUnitDecl(),
				//						clang::SourceLocation(),
				//						clang::SourceLocation(),
				//						clang::DeclarationName(&CI.getASTContext().Idents.get("main")),
				//						CI.getASTContext().getFunctionType(
				//							CI.getASTContext().IntTy, // ������������ ��� (int)
				//							{},                       // ��������� (�����)
				//							clang::FunctionProtoType::ExtProtoInfo()
				//						),
				//						nullptr, // ��� (���� ����)
				//						clang::StorageClass::SC_None
				//					);
				//
				//					// ������� ���� ������� (return 42;)
				//					clang::ReturnStmt* RS = clang::ReturnStmt::Create(
				//						CI.getASTContext(),
				//						clang::SourceLocation(),
				//						clang::IntegerLiteral::Create(
				//							CI.getASTContext(),
				//							llvm::APInt(32, 42), // �������� 42
				//							CI.getASTContext().IntTy,
				//							clang::SourceLocation()
				//						),
				//						nullptr
				//					);
				//
				//					// ������� CompoundStmt (���� �������)
				//					clang::CompoundStmt* CS = clang::CompoundStmt::Create(
				//						CI.getASTContext(),
				//						clang::ArrayRef<clang::Stmt*>(RS),
				//						clang::FPOptionsOverride{},
				//						clang::SourceLocation(),
				//						clang::SourceLocation()
				//					);
				//
				//					// ����������� ���� �������
				//					FD->setBody(CS);
				//
				//					// ���������� C++ ��� �� ������ AST
				//					FD->print(llvm::outs());
				//
				//					// ������� ��������������� ���
				//					R.getEditBuffer(CI.getSourceManager().getMainFileID()).write(llvm::outs());
				//
				//				

				Function::CreateInfo fci{
					.name_ = "GetName",
					.parameters_ = {},
					.returnType_ = "const char*",
					.code_ = "return Common::TypeInfo<" + componentEcsFile->GetName() + ">::GetTypeName();",
					.inlineModifier_ = false,
					.staticModifier_ = true
				};

				auto getNameMethod = std::make_shared<Function>(fci);

				Struct::CreateInfo sci{
					.name_ = componentEcsFile->GetName(),
					.parent_ = "ECS2::IComponent<" + componentEcsFile->GetName() + ">",
					.fields_ = fields,
					.methods_ = { getNameMethod }
				};
				auto structObject = std::make_shared<Struct>(sci);



				namespaceObject->Add(structObject);
			}

			//Edit function.
			{
				Code realization;
				componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
					if (IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
						realization.Add(GenerateTypeImGuiEditCode(
							componentEcsFile->GetLowerName(),
							fieldInfo.GetTypeName(),
							fieldInfo.GetName(),
							fieldInfo.GetName() + "_")
							.NewLine());
					}
					return true;
					});

				Function::CreateInfo fci{
					.name_ = "Edit" + componentEcsFile->GetName(),
					.parameters_ = {
						{ componentEcsFile->GetName() + "*", componentEcsFile->GetLowerName()}
					},
					.returnType_ = "void",
					.code_ = realization,
					.inlineModifier_ = true
				};

				auto editFunction = std::make_shared<Function>(fci);

				namespaceObject->Add(editFunction);
			}

			//Bind function.
			{
				Code realization;
				realization.Add("context.GetGlobalNamespace()");
				realization.NewLine();
				realization.Add("\t.beginClass<" + componentEcsFile->GetName() + ">(\"" + componentEcsFile->GetName() + "\")");
				realization.NewLine();
				componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
					if (IsBindableType(fieldInfo.GetTypeName())) {
						realization.Add("\t.addProperty (\"" + fieldInfo.GetName() + "\", &" + componentEcsFile->GetName() + "::" + fieldInfo.GetName() + "_)");
						realization.NewLine();
					}
					return true;
					});
				realization.Add("\t.endClass();");
				realization.NewLine();

				Function::CreateInfo fci{
					.name_ = "Bind" + componentEcsFile->GetName(),
					.parameters_ = {
						{ "::Lua::Context&", "context" }
					},
					.returnType_ = "void",
					.code_ = realization,
					.inlineModifier_ = true
				};

				auto editFunction = std::make_shared<Function>(fci);

				namespaceObject->Add(editFunction);
			}


			//Add function.
			{
				Code realization;
				if (componentEcsFile->CanBeCreatedFromImGui()) {
					componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
						realization.Add(GenerateTypeImGuiInputVariable(
							fieldInfo.GetTypeName(),
							fieldInfo.GetName())
							.NewLine());
						return true;
						});
				}
				realization.NewLine();
				realization.Add("if (ImGui::CollapsingHeader(\"Create info\")) {");
				{
					realization.NewLine();

					if (componentEcsFile->CanBeCreatedFromImGui()) {
						componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
							std::string variableName;
							if (!(fieldInfo.GetTypeName() == "std::string")) {
								variableName += "&"; // If string we dont need &, because array id is pointer.
							}
							variableName += fieldInfo.GetName();
							Code addRealization =
								GenerateImGuiInputTypeCode(
									fieldInfo.GetName(),
									fieldInfo.GetTypeName(),
									variableName);
							addRealization.ApplyTab();
							realization.Add(addRealization);
							realization.NewLine();
							return true;
							});
					}


					realization.Add("ImGui::Spacing();");

					realization.NewLine();
					realization.Add("\tif (ImGui::Button(\"Add component\")) {");
					{
						realization.NewLine();
						realization.Add("if (!world->IsComponentExist<" + componentEcsFile->GetName() + ">(entityId)) {");
						realization.NewLine();

						realization.Add("world->CreateComponent<" + componentEcsFile->GetName() + ">(entityId");
						if (componentEcsFile->CanBeCreatedFromImGui()) {
							if (componentEcsFile->AreThereFields()) {
								realization.Add(", ");
							}
							componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
								realization.Add(fieldInfo.GetName());
								if (!isLast) {
									realization.Add(", ");
								}
								return true;
								});
						}
						realization.Add(");");
						realization.NewLine();


						realization.Add("}");
					}
					realization.NewLine();
					realization.NewLine();
					realization.Add("}");
				}
				realization.NewLine();
				realization.Add("}");
				Function::CreateInfo fci{
					.name_ = "Add" + componentEcsFile->GetName(),
					.parameters_ = {
						{ "ECS2::World*", "world" },
						{ "ECS2::Entity::Id", "entityId" }
					},
					.returnType_ = "void",
					.code_ = realization,
					.inlineModifier_ = true
				};

				auto editFunction = std::make_shared<Function>(fci);

				namespaceObject->Add(editFunction);
			}

			File::Includes includes{
				{	"imgui.h",
					"ECS2.World.hpp",
					"ECS2.Entity.hpp",
					"ECS2.Component.hpp",
					"Lua.Context.hpp" }
			};

			componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {

				if (fieldInfo.GetTypeName() == "std::string") {
					includes.paths_.push_back("string");
				}
				if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
					includes.paths_.push_back("Common.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
					includes.paths_.push_back("chrono");
				}
				if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
					includes.paths_.push_back("PAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
					includes.paths_.push_back("glm/glm.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
					includes.paths_.push_back("queue");
				}
				if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
					includes.paths_.push_back("vector");
				}
				if (fieldInfo.GetTypeName().find("RAL") != std::string::npos) {
					includes.paths_.push_back("RAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
					includes.paths_.push_back("Geometry.hpp");
				}
				if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
					includes.paths_.push_back("UIAL.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("UI::Window") != std::string::npos) {
					includes.paths_.push_back("UI/OksEngine.UI.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("sf") != std::string::npos) {
					includes.paths_.push_back("SFML/Audio.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
					includes.paths_.push_back("glm/gtc/quaternion.hpp");
				}
				if (fieldInfo.GetTypeName().find("PE") != std::string::npos) {
					includes.paths_.push_back("PE.PhysicsEngine.hpp");
				}
				if (fieldInfo.GetTypeName().find("AsyncResourceSubsystem") != std::string::npos) {
					includes.paths_.push_back("Resources/OksEngine.Resource.Subsystem.hpp");
				}

				return true;
				});

			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path componentHppFileFullPath
				= componentEcsFile->GetPath().parent_path() / ("auto_OksEngine." + componentEcsFile->GetName() + ".hpp");
			std::string componentHppFileFullPathString = componentHppFileFullPath.string();
			std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ componentHppFileFullPathString }, file };

		}

		CreateInfo ci_;
	};


}