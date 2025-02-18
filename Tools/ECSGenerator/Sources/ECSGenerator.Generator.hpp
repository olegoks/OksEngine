#pragma once 

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.CodeFile.hpp>


namespace ECSGenerator {


	inline std::filesystem::path GetSubPath(const std::filesystem::path& path, const std::string& folderFrom) {
		bool sourcesFound = false;
		std::filesystem::path result;
		std::filesystem::path::iterator sourcesIt;
		for (auto it = path.end(); it != path.begin(); --it) {
			if (*it == folderFrom) {
				sourcesIt = it;
				break;
			}
		}
		for (auto it = ++sourcesIt;//skip folderFrom directory
			it != path.end();
			++it) {
			result /= *it;
		}
		return result;
	}

	class SystemFileStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		SystemFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}




		//c:/users/user/desktop/oksengine/sources/engine\sources\common\clock\
		// ->
		//\common\clock\


		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> Generate(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

			File::Includes includes{ };

			includes.paths_.insert("ECS2.hpp");

			std::filesystem::path projectPath = GetSubPath(systemEcsFile->GetPath(), projectContext->includeDirectory_);

			for (auto componentInclude : systemEcsFile->includes_) {
				std::filesystem::path includePath;

				auto componentEcsFile = projectContext->GetEcsFileByName(componentInclude);
				includePath = GetSubPath(componentEcsFile->GetPath().parent_path(), projectContext->includeDirectory_);

				const std::filesystem::path componentHPPIncludePath
					= includePath / ("auto_OksEngine." + componentInclude + ".hpp");
				std::string componentHPPIncludePathString = componentHPPIncludePath.string();
				std::replace(componentHPPIncludePathString.begin(), componentHPPIncludePathString.end(), '\\', '/');
				includes.paths_.insert(componentHPPIncludePathString);
			}

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{

				std::vector<Function::Parameter> components;

				systemEcsFile->ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
					std::string lowerIncludeName = std::string{ (char)std::tolower(include[0]) } + include.substr(1);
					components.push_back({ include + "*", lowerIncludeName });

					return true;
					});

				Function::CreateInfo updateMethodCI{
					.name_ = "Update",
					.parameters_ = components,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				Struct::CreateInfo sci{
					.name_ = systemEcsFile->GetName(),
					.parent_ = "",
					.fields_ = { },
					.methods_ = { updateMethod }
				};
				auto structObject = std::make_shared<Struct>(sci);



				namespaceObject->Add(structObject);
			}
			{
				std::vector<Function::Parameter> parameters;
				parameters.push_back({ "ECS2::World*", "world" });

				Code realization;
				realization.Add(systemEcsFile->GetName() + " " + std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ";");
				realization.NewLine();
				realization.Add("world->ForEachEntity<");
				systemEcsFile->ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
					realization.Add(include);
					if (!isLast) {
						realization.Add(", ");
					}
					return true;
					});
				realization.Add(">(ECS2::ComponentsFilter{}, [&](");
				systemEcsFile->ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
					realization.Add(include + "* " + std::string{ (char)std::tolower(include[0]) } + include.substr(1));
					if (!isLast) {
						realization.Add(", ");
					}
					return true;
					});
				realization.Add("){");
				realization.NewLine();
				realization.Add(std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ".Update(");
				systemEcsFile->ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
					realization.Add(std::string{ (char)std::tolower(include[0]) } + include.substr(1));
					if (!isLast) {
						realization.Add(", ");
					}
					return true;
					});
				realization.Add(");");
				realization.NewLine();
				realization.Add("});");
				realization.NewLine();
				//UpdateClock updateClock;
				//world->ForEachEntity<Clock>(ECS2::ComponentsFilter{}, [&](Clock* clock) {
				//	});
				Function::CreateInfo fci{
					.name_ = "Run" + systemEcsFile->GetName() + "System",
					.parameters_ = parameters,
					.returnType_ = "void",
					.code_ = realization,
					.inlineModifier_ = true

				};
				auto updateMethod = std::make_shared<Function>(fci);

				namespaceObject->Add(updateMethod);
			}

			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path systemHppFileFullPath 
				= systemEcsFile->GetPath().parent_path() / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
			std::string systemHppFileFullPathString = systemHppFileFullPath.string();
			std::replace(systemHppFileFullPathString.begin(), systemHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ systemHppFileFullPathString }, file };

		}

		CreateInfo ci_;
	};


	class ComponentFileStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		ComponentFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> Generate(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedComponentECSFile> componentEcsFile) {

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


				/*[[nodiscard]]
				static const char* GetName() noexcept {
				return Common::TypeInfo<Type>::GetTypeName();
				}*/
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
					includes.paths_.insert("string");
				}
				if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
					includes.paths_.insert("Common.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
					includes.paths_.insert("chrono");
				}
				if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
					includes.paths_.insert("PAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
					includes.paths_.insert("glm/glm.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
					includes.paths_.insert("queue");
				}
				if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
					includes.paths_.insert("vector");
				}
				if (fieldInfo.GetTypeName().find("RAL") != std::string::npos) {
					includes.paths_.insert("RAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
					includes.paths_.insert("Geometry.hpp");
				}
				if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
					includes.paths_.insert("UI/OksEngine.UIAL.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("sf") != std::string::npos) {
					includes.paths_.insert("SFML/Audio.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
					includes.paths_.insert("glm/gtc/quaternion.hpp");
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

	class CodeStructureGenerator {
	public:


		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> Generate(std::shared_ptr<ProjectContext> projectContext) {

			std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;


			ComponentFileStructureGenerator::CreateInfo ci{
						.includeDirectory_ = projectContext->includeDirectory_
			};

			projectContext->ForEachComponentEcsFile(
				[&](std::shared_ptr<ParsedComponentECSFile> componentEcsFile) {

					ComponentFileStructureGenerator generator{ ci };
					auto file = generator.Generate(projectContext, componentEcsFile);
					files.push_back(file);
				});

			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedSystemECSFile> componentEcsFile) {
					SystemFileStructureGenerator::CreateInfo ci{
						.includeDirectory_ = projectContext->includeDirectory_
					};
					SystemFileStructureGenerator generator{ ci };
					auto file = generator.Generate(projectContext, componentEcsFile);
					files.push_back(file);
				});



			//Generate OksEngine.Components.hpp files For COMPONENTS.
			for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

				//Generate components includes for components in the subdirectory 
				File::Includes includes{};
				std::filesystem::path componentIncludesFilePath;
				for (auto componentParsedEcsFile : componentsSystems.components_) {
					std::filesystem::path componentFullPath = componentParsedEcsFile->GetPath();
					componentIncludesFilePath = componentFullPath.parent_path();
					std::filesystem::path componentIncludePath = GetSubPath(componentFullPath.parent_path(), projectContext->includeDirectory_);

					componentIncludePath /= ("auto_OksEngine." + componentParsedEcsFile->GetName() + ".hpp");
					includes.paths_.insert(componentIncludePath);
				}

				//generate Include OksEngine.Components.hpp files from subdirectories.
				for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
					if (path.string().starts_with(categoryPath.string())) {
						if (path == categoryPath) {
							continue;
						}
						auto pathIt = path.begin();
						bool subdirFound = true;
						for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
							if (*it != *pathIt) {
								subdirFound = false;
								break;
							}
							++pathIt;
						}
						if (subdirFound) {
							//Subdirectory found!
							std::filesystem::path subdirPath;
							++pathIt;
							for (auto it = path.begin(); it != pathIt; ++it) {
								subdirPath /= *it;
							}
							std::filesystem::path componentIncludePath = GetSubPath(subdirPath, projectContext->includeDirectory_);

							componentIncludePath /= ("auto_OksEngine.Components.hpp");
							includes.paths_.insert(componentIncludePath);
						}
					}
				}

				File::CreateInfo fci{
					.isHpp_ = true,
					.includes_ = includes,
					.base_ = nullptr
				};

				auto categoryComponentsIncludeFile = std::make_shared<File>(fci);
				std::filesystem::path componentIncludesFullFilePath = componentIncludesFilePath / "auto_OksEngine.Components.hpp";

				std::string componentIncludesFullFilePathString = componentIncludesFullFilePath.string();
				std::replace(componentIncludesFullFilePathString.begin(), componentIncludesFullFilePathString.end(), '\\', '/');
				files.push_back({ std::filesystem::path{ componentIncludesFullFilePathString }, categoryComponentsIncludeFile });
			}



			//Generate OksEngine.Components.hpp files For Systems.
			for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

				if (componentsSystems.systems_.empty()) {
					continue;
				}
				//Generate components includes for components in the subdirectory 
				File::Includes includes{};
				std::filesystem::path systemsIncludesFilePath;
				for (auto systemParsedEcsFile : componentsSystems.systems_) {
					std::filesystem::path systemFullPath = systemParsedEcsFile->GetPath();
					systemsIncludesFilePath = systemFullPath.parent_path();
					std::filesystem::path systemIncludePath = GetSubPath(systemFullPath.parent_path(), projectContext->includeDirectory_);

					systemIncludePath /= ("auto_OksEngine." + systemParsedEcsFile->GetName() + ".hpp");
					includes.paths_.insert(systemIncludePath);
				}

				//generate Include OksEngine.Components.hpp files from subdirectories.
				for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
					if (componentsSystems.systems_.empty()) {
						continue;
					}
					if (path.string().starts_with(categoryPath.string())) {
						if (path == categoryPath) {
							continue;
						}
						auto pathIt = path.begin();
						bool subdirFound = true;
						for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
							if (*it != *pathIt) {
								subdirFound = false;
								break;
							}
							++pathIt;
						}
						if (subdirFound) {
							//Subdirectory found!
							std::filesystem::path subdirPath;
							++pathIt;
							for (auto it = path.begin(); it != pathIt; ++it) {
								subdirPath /= *it;
							}
							std::filesystem::path systemIncludePath = GetSubPath(subdirPath, projectContext->includeDirectory_);

							systemIncludePath /= ("auto_OksEngine.Systems.hpp");
							includes.paths_.insert(systemIncludePath);
						}
					}
				}

				File::CreateInfo fci{
					.isHpp_ = true,
					.includes_ = includes,
					.base_ = nullptr
				};

				auto categorySystemIncludeFile = std::make_shared<File>(fci);
				std::filesystem::path systemIncludesFullFilePath = systemsIncludesFilePath / "auto_OksEngine.Systems.hpp";

				std::string systemIncludesFullFilePathString = systemIncludesFullFilePath.string();
				std::replace(systemIncludesFullFilePathString.begin(), systemIncludesFullFilePathString.end(), '\\', '/');
				files.push_back({ std::filesystem::path{ systemIncludesFullFilePathString }, categorySystemIncludeFile });
			}

			return files;
		}

	private:
	};

	class CodeGenerator {
	public:

		static Code GenerateInclude(const std::filesystem::path& path) {
			Code code{ "#include <" + path.string() + ">" };
			code.NewLine();
			return code;
		}

		Code Generate(std::shared_ptr<Struct> structObject) {
			Code code;
			code.Add("struct " + structObject->GetName() + " ");
			if (structObject->HasParent()) {
				code.Add(": " + structObject->GetParentName() + " ");
			}
			code.Add("{");
			code.NewLine();
			code.NewLine();

			Code structRealization;
			{
				if (structObject->ci_.defaultConstructor_) {
					//Default constructor.
					structRealization.Add(structObject->GetName() + "() { }");
					structRealization.NewLine();
				}

				if (structObject->AreThereFields()) {


					//Fields
					structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
						structRealization.Add(field.type_ + " " + field.name_ + "_;");
						structRealization.NewLine();
						});

					structRealization.NewLine();

					if (structObject->ci_.constructor_) {
						//Main constructor.
						structRealization.Add(structObject->GetName() + "(");
						structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
							structRealization.Add(field.type_ + ((!field.copyable_) ? ("&& ") : (" ")) + field.name_);
							if (!isLast) {
								structRealization.Add(", ");
							}
							return true;
							});
						structRealization.Add(") : ");
						structRealization.NewLine();
						structRealization.NewLine();

						structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
							structRealization.Add(field.name_ + "_{ " 
								+ ((!field.copyable_) ? ("std::move(") : ("")) +
								field.name_ 
								+ ((!field.copyable_) ? (")") : ("")) + " }");
							if (!isLast) {
								structRealization.Add(",");
								structRealization.NewLine();
							}
							return true;
							});
						structRealization.Add("{ }");
						structRealization.NewLine();
						structRealization.NewLine();
					}



				}

				for (auto functionObject : structObject->ci_.methods_) {
					Code code;
					code.Add(Generate(functionObject));
					structRealization.Add(code);
				}
				structRealization.ApplyTab();
				code.Add(structRealization);
			}
			code.Add("};");

			code.NewLine();

			return code;
		}

		Code Generate(std::shared_ptr<Namespace> namespaceObject) {
			Code code;

			code.Add("namespace " + namespaceObject->GetName() + " {");
			code.NewLine();
			code.NewLine();
			for (auto base : namespaceObject->base_) {
				Code namespaceCode = Generate(base);
				namespaceCode.ApplyTab();
				code.Add(namespaceCode);
			}
			code.NewLine();
			code.Add("}");
			code.NewLine();
			return code;
		}

		Code Generate(std::shared_ptr<Function> functionObject) {
			Code code;
			if (!functionObject->ci_.isPrototype_) {
				code.Add((functionObject->ci_.staticModifier_) ? ("static ") : (""));
				code.Add((functionObject->ci_.inlineModifier_) ? ("inline ") : (""));
				code.Add(functionObject->ci_.returnType_ + " " + functionObject->ci_.name_ + "(");
				for (Common::Index i = 0; i < functionObject->ci_.parameters_.size(); i++) {
					const Function::Parameter& parameter = functionObject->ci_.parameters_[i];
					code.Add(parameter.inputType_ + " " + parameter.valueName_);
					if (i < functionObject->ci_.parameters_.size() - 1) {
						code.Add(", ");
					}
				}
				code.Add("){");
				code.NewLine();
				Code realization = functionObject->ci_.code_;
				realization.ApplyTab();
				code.Add(realization);
				code.NewLine();
				code.Add("};");
				code.NewLine();
			}
			else {
				code.Add((functionObject->ci_.inlineModifier_) ? ("inline") : ("") + functionObject->ci_.returnType_ + " " + functionObject->ci_.name_ + "(");
				for (Common::Index i = 0; i < functionObject->ci_.parameters_.size(); i++) {
					const Function::Parameter& paramter = functionObject->ci_.parameters_[i];
					code.Add(paramter.inputType_ + " " + paramter.valueName_);
					if (i < functionObject->ci_.parameters_.size() - 1) {
						code.Add(", ");
					}
				}
				code.Add(");");
				code.NewLine();
				return code;
			}
			return code;
		}

		Code Generate(std::shared_ptr<Base> base) {
			Code code;
			if (base == nullptr) {
				return code;
			}
			if (base->GetType() == Base::Type::Namespace) {
				auto namespaceObject = std::dynamic_pointer_cast<Namespace>(base);
				code = Generate(namespaceObject);

			}
			else if (base->GetType() == Base::Type::Struct) {
				auto structObject = std::dynamic_pointer_cast<Struct>(base);
				code = Generate(structObject);
			}
			else if (base->GetType() == Base::Type::Function) {
				auto functionObject = std::dynamic_pointer_cast<Function>(base);
				code = Generate(functionObject);
			}
			code.NewLine();
			return code;
		}

		std::shared_ptr<CodeFile> GenerateCode(std::shared_ptr<File> fileStructure) {
			return Generate(fileStructure);
		}

		std::shared_ptr<CodeFile> Generate(std::shared_ptr<File> fileStructure) {

			auto codeFile = std::make_shared<CodeFile>();

			if (fileStructure->IsHpp()) {
				Code code;
				code.Add("#pragma once");
				code.NewLine();
				code.NewLine();
				codeFile->AddCode(code);
			}
			//Includes.
			{
				Code includes;
				fileStructure->ForEachInclude([&](const std::filesystem::path& path) {
					includes.Add(GenerateInclude(path));

					});
				includes.NewLine();
				codeFile->AddCode(includes);
			}
			const Code code = Generate(fileStructure->GetBase());
			//codeFile->path_ = fileStructure->cr
			codeFile->AddCode(code);
			return codeFile;
		}

		std::shared_ptr<CodeFile> GenerateComponentCodeFile() {


			//std::string cxxECSCode = "//THIS FILE IS AUTOGENERATED!!!\n";
//cxxECSCode += "#pragma once\n";
//cxxECSCode += "\n";
//cxxECSCode += "#include <OksEngine.ECS.Component.hpp>\n";
//cxxECSCode += "#include <imgui.h>\n";
//cxxECSCode += "namespace OksEngine{\n\n";

			//std::string name = componentInfo.GetName();
			//std::string lowerName = componentInfo.GetLowerName();

			//cxxECSCode += GenerateComponentStructDefinition(componentInfo);

			//cxxECSCode += "\n";

			////Generate Edit function. 
			//cxxECSCode += GenerateImGuiEditFunction(componentInfo);

			////Generate Bind function. 
			//cxxECSCode += "template<>\n";
			//cxxECSCode += "inline void Bind<" + name + ">(::Lua::Context & context) {\n";
			//cxxECSCode += "\tcontext.GetGlobalNamespace()\n";
			//cxxECSCode += "\t\t.beginClass<" + name + ">(\"" + name + "\")\n";
			//for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
			//	luabridge::LuaRef field = it.value();
			//	std::string type = field["type"].cast<std::string>().value();
			//	std::string fieldName = field["name"].cast<std::string>().value();
			//	if (IsBindableType(type)) {
			//		cxxECSCode += "\t\t.addProperty (\"" + fieldName + "\", &" + name + "::" + fieldName + "_)\n";
			//	}
			//}
			//cxxECSCode += "\t\t.endClass();\n";
			//cxxECSCode += "};\n\n";

			//cxxECSCode += GenerateAddFunction(componentInfo);


			//std::make_shared<ComponentInfo>(componentInfo);
			////Namespace OksEngine end.
			//cxxECSCode += "\n};\n";
			//return cxxECSCode;
			return nullptr;
		}



	};



}