#pragma once 

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.CodeFile.hpp>

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  


#include <graphviz/gvc.h>

#include <clang/Format/Format.h>
#include <clang/Tooling/Core/Replacement.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Path.h>


namespace ECSGenerator {



	class SystemFileStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		SystemFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::filesystem::path GenerateCPPFilePath(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

		}



		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateSystemRealization(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


			File::Includes includes{ };


			const std::string systemName = systemEcsFile->GetName();
			std::filesystem::path systemsIncludesFilePath;
			std::filesystem::path systemFullPath = systemEcsFile->GetPath();
			systemsIncludesFilePath = systemFullPath.parent_path();
			std::filesystem::path systemIncludePath = GetSubPath(
				systemFullPath.parent_path(),
				projectContext->includeDirectory_,
				ResultRange::FromStartFolderToEnd,
				SearchDirection::FromEndToBegin,
				true);

			systemIncludePath /= ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
			includes.paths_.push_back(systemIncludePath);


			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;
				//Update method.
				std::vector<Function::Parameter> updateMethodParameters;


				//If system  process all entities need only one paramter Entity::Id
				if (systemEcsFile->ci_.processAllEntities_) {
					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
				}


				for (Common::Index i = 0; i < systemEcsFile->ci_.entities_.size(); i++) {

					std::string entityIdString = "";
					if (systemEcsFile->ci_.entities_.size() == 2) {
						entityIdString = (i == 0) ? ("1") : ("2");
					}

					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
					const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.entities_[i];
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
						updateMethodParameters.push_back({
							((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
							lowerIncludeName });

						return true;
						});
				}

				Function::CreateInfo updateMethodCI{
					.name_ = systemEcsFile->GetName() + "::Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				namespaceObject->Add(updateMethod);
			}

			File::CreateInfo fci{
				.isHpp_ = false,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path componentHppFileFullPath
				= systemEcsFile->GetPath().parent_path() / ("OksEngine." + systemEcsFile->GetName() + ".cpp");
			std::string componentHppFileFullPathString = componentHppFileFullPath.string();
			std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ componentHppFileFullPathString }, file };

		}


		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

			// // ������� CompilerInstance
			// clang::CompilerInstance CI;
			// CI.createDiagnostics();
			// CI.getLangOpts().CPlusPlus = true; // �������� ��������� C++

			// // ������� ASTContext
			// CI.createASTContext();

			// // ������� Rewriter ��� ��������� ����
			// clang::Rewriter R;
			// R.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());

			// // ������� TranslationUnitDecl (�������� ���� AST)
			// clang::ASTContext& AST = CI.getASTContext();
			// clang::TranslationUnitDecl* TU = AST.getTranslationUnitDecl();

			// // ��������� ��������� �������������
			// R.InsertText(clang::SourceLocation(), "#pragma once\n");
			// R.InsertText(clang::SourceLocation(), "#include <Common.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.Component.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.Entity.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.World.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <Lua.Context.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <glm/glm.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <glm/gtc/quaternion.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <imgui.h>\n");
			// R.InsertText(clang::SourceLocation(), "#include <string>\n");
			// R.InsertText(clang::SourceLocation(), "#include <vector>\n\n");

			// // ������� ������������ ���� OksEngine
			// clang::NamespaceDecl* OksEngine = clang::NamespaceDecl::Create(
			// 	AST, TU, false, clang::SourceLocation(), clang::SourceLocation(),
			// 	&AST.Idents.get("OksEngine"), nullptr, false);

			// // ������� ��������� Animation
			// clang::RecordDecl* Animation = clang::CXXRecordDecl::Create(
			// 	AST, clang::TagDecl::TagKind::Struct, OksEngine, clang::SourceLocation(),
			// 	clang::SourceLocation(), &AST.Idents.get("Animation"));

			// // ��������� ������� ����� ECS2::IComponent<Animation>
			// clang::QualType BaseType = AST.getTemplateSpecializationTypeInfo(
			// 	AST.getRecordType(AST.getRecordType(AST.getTypeDeclType(
			// 		AST.getTypeDecl(TU, clang::SourceLocation(), &AST.Idents.get("ECS2")))),
			// 		{ AST.getTypeDeclType(Animation) });
			// clang::CXXBaseSpecifier * BaseSpec = clang::CXXBaseSpecifier::Create(
			// 	AST, clang::SourceLocation(), false, false, BaseType, clang::SourceLocation(),
			// 	clang::SourceLocation());
			// Animation->setBases(BaseSpec, 1);

			// // ��������� ����������� �� ���������
			// clang::CXXConstructorDecl * DefaultCtor = clang::CXXConstructorDecl::Create(
			// 	AST, Animation, clang::SourceLocation(), clang::DeclarationName(), clang::QualType(),
			// 	nullptr, clang::Function::FunctionKind::FK_DefaultConstructor, false, false,
			// 	false, false);
			// DefaultCtor->setAccess(clang::AccessSpecifier::AS_public);
			// Animation->addDecl(DefaultCtor);

			// // ��������� ����
			// clang::FieldDecl * NameField = clang::FieldDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	&AST.Idents.get("name_"), AST.getStdStringType(), nullptr, nullptr,
			// 	false, false);
			// NameField->setAccess(clang::AccessSpecifier::AS_private);
			// Animation->addDecl(NameField);

			// clang::FieldDecl * DurationField = clang::FieldDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	&AST.Idents.get("durationInTicks_"), AST.getDoubleType(), nullptr,
			// 	nullptr, false, false);
			// DurationField->setAccess(clang::AccessSpecifier::AS_private);
			// Animation->addDecl(DurationField);

			// // ��������� ����������� � �����������
			// QualType ParamTypes[] = {
			// 	AST.getStdStringType(),
			// 	AST.getDoubleType(),
			// 	AST.FloatTy,
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	),
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	),
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	)
			// };
			// CXXConstructorDecl * ParamCtor = CXXConstructorDecl::Create(
			// 	AST, Animation, SourceLocation(), DeclarationName(), QualType(),
			// 	nullptr, Function::FunctionKind::FK_Constructor, false, false, false,
			// 	false);
			// ParamCtor->setAccess(AccessSpecifier::AS_public);
			// Animation->addDecl(ParamCtor);

			// // ��������� ����� GetName
			// FunctionDecl * GetNameMethod = FunctionDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	DeclarationName(&AST.Idents.get("GetName")), AST.getPointerType(AST.CharTy),
			// 	nullptr, Function::FunctionKind::FK_MemberFunction, false, false, false,
			// 	false);
			// GetNameMethod->setAccess(AccessSpecifier::AS_public);
			// Animation->addDecl(GetNameMethod);

			// // ���������� ���
			// Animation->print(R.getEditBuffer(CI.getSourceManager().getMainFileID()));

			// // ������� ��������������� ���
			// R.getEditBuffer(CI.getSourceManager().getMainFileID()).write(llvm::outs());

			File::Includes includes{ };
			includes.paths_.push_back("ECS2.hpp");
			includes.paths_.push_back("chrono");
			if (!systemEcsFile->ci_.createEntityComponents_.empty()) {
				for (auto& createEntityComponent : systemEcsFile->ci_.createEntityComponents_) {
					
					auto componentEcsFile = projectContext->GetEcsFileByName(createEntityComponent);
					const std::filesystem::path includePath = GetSubPath(
						componentEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);
	/*				includePath = GetSubPathFromFolderToEnd(componentEcsFile->GetPath().parent_path(), projectContext->includeDirectory_);

					const std::filesystem::path componentHPPIncludePath
						= includePath / ;
					std::string componentHPPIncludePathString = componentHPPIncludePath.string();
					std::replace(componentHPPIncludePathString.begin(), componentHPPIncludePathString.end(), '\\', '/');*/
					includes.paths_.push_back(includePath / ("auto_OksEngine." + createEntityComponent + ".hpp"));
				}
			}

			for (auto& entity : systemEcsFile->ci_.entities_) {

				//Generate includes for components that system uses as input.
				for (auto componentInclude : entity.includes_) {
					auto componentEcsFile = projectContext->GetEcsFileByName(componentInclude.name_);
					const std::filesystem::path includePath = GetSubPath(
						componentEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);

					//const std::filesystem::path componentHPPIncludePath
					//	= includePath / ;
					//std::string componentHPPIncludePathString = componentHPPIncludePath.string();
					//std::replace(componentHPPIncludePathString.begin(), componentHPPIncludePathString.end(), '\\', '/');
					includes.paths_.push_back(includePath / ("auto_OksEngine." + componentInclude.name_ + ".hpp"));
				}

				//Generate includes for components that system creates.
				for (auto componentCreates : entity.creates_) {
					auto componentEcsFile = projectContext->GetEcsFileByName(componentCreates);
					const std::filesystem::path includePath = GetSubPath(
						componentEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);
					includes.paths_.push_back(includePath / ("auto_OksEngine." + componentCreates + ".hpp"));
				}

				//Generate includes for components that system creates.
				for (auto componentExclude : entity.excludes_) {
					auto componentEcsFile = projectContext->GetEcsFileByName(componentExclude);
					//includePath = GetSubPathFromFolderToEnd(componentEcsFile->GetPath().parent_path(), projectContext->includeDirectory_);
					const std::filesystem::path includePath = GetSubPath(
						componentEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);
					//const std::filesystem::path componentHPPIncludePath
					//	= includePath / ;
					//std::string componentHPPIncludePathString = componentHPPIncludePath.string();
					//std::replace(componentHPPIncludePathString.begin(), componentHPPIncludePathString.end(), '\\', '/');
					includes.paths_.push_back(includePath / ("auto_OksEngine." + componentExclude + ".hpp"));
				}

			}
			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;
				//Update method.
				std::vector<Function::Parameter> updateMethodParameters;

				//If system  process all entities need only one paramter Entity::Id
				if (systemEcsFile->ci_.processAllEntities_) {
					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
				}

				for (Common::Index i = 0; i < systemEcsFile->ci_.entities_.size(); i++) {

					std::string entityIdString = "";
					if (systemEcsFile->ci_.entities_.size() == 2) {
						entityIdString = (i == 0) ? ("1") : ("2");
					}

					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
					const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.entities_[i];
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
						updateMethodParameters.push_back({
							((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
							lowerIncludeName });

						return true;
						});
				}

				//Add entity index to component names if there are two components with the same name.



				Function::CreateInfo updateMethodCI{
					.name_ = "Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "world_->CreateEntity();",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

				//CreateEntity<> method.
				Function::CreateInfo createArchetypeEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "world_->CreateEntity<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto createArchetypeEntityMethod = std::make_shared<Function>(createArchetypeEntityCI);

				//CreateComponent method.
				Function::CreateInfo createComponentCI{
					.name_ = "CreateComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = "world_->CreateComponent<Component>(entityId, std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto createComponentMethod = std::make_shared<Function>(createComponentCI);

				Struct::CreateInfo sci{
					.name_ = systemEcsFile->GetName(),
					.parent_ = "",
					.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
					.methods_ = { updateMethod, createComponentMethod, createDynamicEntityMethod, createArchetypeEntityMethod }
				};
				auto structObject = std::make_shared<Struct>(sci);



				namespaceObject->Add(structObject);
			}
			{
				std::vector<Function::Parameter> parameters;
				parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

				Code realization;
				if (!systemEcsFile->ci_.processAllEntities_) {
					realization.Add(systemEcsFile->GetName() + " " + std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + "{ world };");
					realization.NewLine();

					//Excludes
					realization.Add("ECS2::ComponentsFilter excludeEntity1;");
					realization.NewLine();
					if (!systemEcsFile->ci_.entities_[0].excludes_.empty()) {
						realization.Add("excludeEntity1.SetBits<");
					}
					for (Common::Index i = 0; i < systemEcsFile->ci_.entities_[0].excludes_.size(); ++i) {
						realization.Add(systemEcsFile->ci_.entities_[0].excludes_[i]);
						if (i != systemEcsFile->ci_.entities_[0].excludes_.size() - 1) {
							realization.Add(", ");
						}

					}
					if (!systemEcsFile->ci_.entities_[0].excludes_.empty()) {
						realization.Add(">();");
						realization.NewLine();
					}

					realization.Add("world->ForEachEntity<");
					realization.NewLine();
					systemEcsFile->ci_.entities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						realization.Add(include.name_);
						if (!isLast) {
							realization.Add(", ");
						}
						realization.NewLine();
						return true;
						});
					realization.Add(">(excludeEntity1, [&](");
					realization.Add("ECS2::Entity::Id entity1Id, ");
					systemEcsFile->ci_.entities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
						if (!isLast) {
							realization.Add(", ");
						}
						realization.NewLine();
						return true;
						});
					realization.Add("){");
					realization.NewLine();

					if (systemEcsFile->ci_.entities_.size() == 1) {
						//Call update method
						realization.Add(std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ".Update(");
						realization.Add("entity1Id");
						realization.Add(", ");
						realization.NewLine();
						systemEcsFile->ci_.entities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
							if (!isLast) {
								realization.Add(", ");
							}
							realization.NewLine();
							return true;
							});
						realization.Add(");");
						realization.NewLine();

					}
					else {
						//Excludes
						realization.Add("ECS2::ComponentsFilter excludeEntity2;");
						realization.NewLine();
						if (!systemEcsFile->ci_.entities_[1].excludes_.empty()) {
							realization.Add("excludeEntity2.SetBits<");
						}
						for (Common::Index i = 0; i < systemEcsFile->ci_.entities_[1].excludes_.size(); ++i) {
							realization.Add(systemEcsFile->ci_.entities_[1].excludes_[i]);
							if (i != systemEcsFile->ci_.entities_[1].excludes_.size() - 1) {
								realization.Add(", ");
							}

						}
						if (!systemEcsFile->ci_.entities_[1].excludes_.empty()) {
							realization.Add(">();");
							realization.NewLine();
						}

						realization.Add("world->ForEachEntity<");
						realization.NewLine();
						systemEcsFile->ci_.entities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(include.name_);
							if (!isLast) {
								realization.Add(", ");

							}
							realization.NewLine();
							return true;
							});
						realization.Add(">(excludeEntity2, [&](");
						realization.Add("ECS2::Entity::Id entity2Id, ");
						realization.NewLine();
						systemEcsFile->ci_.entities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
							if (!isLast) {
								realization.Add(", ");
							}
							return true;
							});
						realization.Add("){");
						realization.NewLine();

						//Call update method
						realization.Add(std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ".Update(");
						realization.Add("entity1Id");
						realization.Add(", ");
						realization.NewLine();
						systemEcsFile->ci_.entities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
							if (!isLast) {
								realization.Add(", ");
							}
							return true;
							});

						realization.Add(", ");
						realization.Add("entity2Id");
						realization.Add(", ");
						systemEcsFile->ci_.entities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
							if (!isLast) {
								realization.Add(", ");
							}
							return true;
							});


						realization.Add(");");
						realization.NewLine();
						realization.Add("});");
						realization.NewLine();
					}

					realization.Add("});");
					realization.NewLine();
				}
				else {
					realization.Add(systemEcsFile->GetName() + " " + std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + "{ world };");
					realization.NewLine();
					realization.Add("world->ForEachEntity([&](ECS2::Entity::Id entityId){");
					realization.Add(std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ".Update(");
					realization.Add("entityId);");
					realization.Add("});");

				}
				Function::CreateInfo fci{
					.name_ = systemEcsFile->GetName() + "System",
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
				//				}
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


	void dfs(Agnode_t* node, std::unordered_map<Agnode_t*, bool>& visited, std::vector<Agnode_t*>& component, Agraph_t* g) {
		visited[node] = true;
		component.push_back(node);

		for (Agedge_t* edge = agfstout(g, node); edge; edge = agnxtout(g, edge)) {
			Agnode_t* neighbor = aghead(edge);
			if (!visited[neighbor]) {
				dfs(neighbor, visited, component, g);
			}
		}

		for (Agedge_t* edge = agfstin(g, node); edge; edge = agnxtin(g, edge)) {
			Agnode_t* neighbor = agtail(edge);
			if (!visited[neighbor]) {
				dfs(neighbor, visited, component, g);
			}
		}
	}

	std::vector<std::vector<Agnode_t*>> findConnectedComponents(Agraph_t* g) {
		std::unordered_map<Agnode_t*, bool> visited;
		std::vector<std::vector<Agnode_t*>> clusters;

		for (Agnode_t* node = agfstnode(g); node; node = agnxtnode(g, node)) {
			if (!visited[node]) {
				std::vector<Agnode_t*> component;
				dfs(node, visited, component, g);
				clusters.push_back(component);
			}
		}

		return clusters;
	}

	//Adds new systems to .dot file if needed.
	class SystemCallsGraphDescriptionGenerator {
	public:

		Agraph_t* CreateSystemsGraph(std::shared_ptr<ProjectContext> projectContext) {

			// �������� ������ �����
			Agraph_t* g = agopen((char*)"G", Agstrictundirected, nullptr);

			projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedSystemECSFile> system) {

				//Do not add system that will be call manualy.
				if (system->ci_.processAllEntities_) {
					return;
				}
				Agnode_t* systemNode = agnode(g, (char*)system->GetName().c_str(), 1);
				agsafeset(systemNode, (char*)"shape", (char*)"rect", (char*)"");
				for (auto& entity : system->ci_.entities_) {
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {

						projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedSystemECSFile> maybeDependenceSystem) {
							//Do not add system that will be call manualy.
							if (system->ci_.processAllEntities_) {
								return;
							}
							if (system == maybeDependenceSystem) { return; }
							if (system->GetName() == "UpdateDriverIndexBuffer" && maybeDependenceSystem->GetName() == "CreateUniformBuffer") {
								systemNode = systemNode;
							}
							Agnode_t* maybeDependenceSystemNode = agnode(g, (char*)maybeDependenceSystem->GetName().c_str(), 1);
							agsafeset(maybeDependenceSystemNode, (char*)"shape", (char*)"rect", (char*)"");
							for (auto& entity : maybeDependenceSystem->ci_.entities_) {
								entity.ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
									if (include.name_ == dependenceSystemInclude.name_/* && dependenceSystemInclude.name_ == "RenderDriver"*/) {
										if (include.readonly_ != dependenceSystemInclude.readonly_ || !(include.readonly_ && dependenceSystemInclude.readonly_)) {
											Agedge_t* edge = agedge(g, systemNode, maybeDependenceSystemNode, nullptr, 0);
											if (edge == nullptr) {
												agedge(g, systemNode, maybeDependenceSystemNode, nullptr, 1);
												//	//agsafeset(e, (char*)"dir", (char*)"both", (char*)""); 
											}
										}
									}
									return true;
									});
							}

							});

						return true;
						});
				}

				});

			return g;

		}

		std::vector<std::vector<Agnode_t*>> FindClusters(Agraph_t* g) {

			std::vector<std::string> graphvizColors = {
	"white", "red", "green", "blue", "yellow",
	"cyan", "magenta", "lightgray", "darkgray", "lightred",
	"lightgreen", "lightblue", "darkred", "darkgreen", "darkblue",
	"pink", "orange", "brown", "purple", "violet",
	"turquoise", "goldenrod ", "salmon", "khaki", "plum",
	"coral", "forestgreen", "olive", "navy", "indigo"
			};

			std::vector<std::vector<Agnode_t*>> clusters = findConnectedComponents(g);

			// ������ ���������
			for (size_t i = 0; i < clusters.size(); ++i) {


				std::cout << "Cluster " << i + 1 << ": ";
				for (Agnode_t* node : clusters[i]) {
					char* color = (i < graphvizColors.size()) ? ((char*)graphvizColors[i].c_str()) : ((char*)"red");
					agsafeset(node, (char*)"style", (char*)"filled", (char*)"");
					agsafeset(node, (char*)"fillcolor", (char*)color, "");
					std::cout << agnameof(node) << " ";
				}
				std::cout << std::endl;
			}

			return clusters;
		}

		std::shared_ptr<OS::TextFile> GenerateGraphText(Agraph_t* g, std::shared_ptr<ProjectContext> projectContext) {


			//Parse .dot
			GVC_t* gvc = gvContext();


			//Get path
			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath;

			std::filesystem::path::iterator includeDirIt;
			for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
				auto folder = *it;
				if (folder == projectContext->includeDirectory_) {
					includeDirIt = it;
					break;
				}
			}

			for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
				includeDirFullPath /= *it;
			}

			includeDirFullPath /= *includeDirIt;
			//Get path


			auto dotfile = std::make_shared<OS::TextFile>(includeDirFullPath / "auto_ECSSystemsGraph.dot");

			char* dotData = nullptr;
			unsigned int length = 0;

			//agsafeset(g, (char*)"rankdir", (char*)"LR", (char*)"");
			//agsafeset(g, (char*)"splines", (char*)"ortho", (char*)"");
			//agsafeset(g, (char*)"ep", (char*)"0.5", (char*)"");  // ���������� ����� ������
			//agsafeset(g, (char*)"ranksep", (char*)"10 equally", (char*)""); // ���������� ����� �������


			gvLayout(gvc, g, "dot");
			gvRenderData(gvc, g, "dot", &dotData, &length);

			agclose(g);
			gvFreeContext(gvc);
			dotfile->Create();
			std::string dotText{ dotData };
			(*dotfile) << dotText;
			dotfile->Close();

			return dotfile;
		}
	};

	class CodeStructureGenerator {
	public:

		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>>
			GenerateRunSystemsFiles(std::vector<std::vector<Agnode_t*>> clusters, std::shared_ptr<ProjectContext> projectContext) {

			using System = std::string;
			using Component = std::string;

			//Components and which systems uses them.
			std::map<System, std::set<Component>> componentSystem;
			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedSystemECSFile> componentEcsFile) {

					const std::string systemName = componentEcsFile->GetName();
					for (auto& entity : componentEcsFile->ci_.entities_) {
						entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							if (componentSystem.find(include.name_) != componentSystem.end()) {
								componentSystem.insert({});
							}
							componentSystem[include.name_].insert(systemName);
							return true;
							});
					}
				});

			std::map<std::string, std::set<std::string>> systemComponents;

			for (auto& [component, systems] : componentSystem) {
				for (const auto& system : systems) {
					systemComponents[system].insert(component);
				}
			}

			struct Thread {
				std::map<System, std::set<Component>> systems_;
			};



			std::vector<Thread> threads;

			std::vector<Thread> mainThreads;

			for (auto& clusterSystems : clusters) {
				bool isMainThread = false;
				for (Agnode_t* systemNode : clusterSystems) {
					std::string systemName = agnameof(systemNode);
					projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
						if (systemName == systemEcsFile->GetName() && systemEcsFile->ci_.mainThreadCall_) {
							isMainThread = true;
						}
						});

				}
				Thread clusterThread;
				for (Agnode_t* systemNode : clusterSystems) {
					std::string systemName = agnameof(systemNode);

					clusterThread.systems_[systemName] = componentSystem[systemName];

				}
				if (isMainThread) {
					mainThreads.push_back(clusterThread);
				}
				else {
					threads.push_back(clusterThread);
				}
			}

			//Group threads systems to get less threads.
			{
				const Common::UInt64 threadsNumber = 6;

				auto optimize = [](std::vector<Thread>& threads) {

					std::sort(threads.begin(), threads.end(), [](const Thread& first, const Thread& second) {

						return first.systems_.size() > second.systems_.size();

						});
					auto lastIt = --threads.end();
					auto preLastIt = lastIt--;
					for (auto& [system, components] : lastIt->systems_) {
						preLastIt->systems_[system] = components;
					}
					threads.erase(lastIt);
					};

				while (threads.size() > threadsNumber) {

					optimize(threads);

				}
			}

			std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;

			using namespace std::string_literals;

			//Generate .CPP.

			//Generate Includes.
			File::Includes cppIncludes{ };
			//Must be first to escape 
			//error C1189: #error:  WinSock.h has already been included
			cppIncludes.paths_.push_back("auto_OksEngine.RunSystems.hpp");

			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

					const std::string systemName = systemEcsFile->GetName();
					std::filesystem::path systemsIncludesFilePath;
					std::filesystem::path systemFullPath = systemEcsFile->GetPath();
					systemsIncludesFilePath = systemFullPath.parent_path();
					std::filesystem::path systemIncludePath = GetSubPath(
						systemFullPath.parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);

					systemIncludePath /= ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
					cppIncludes.paths_.push_back(systemIncludePath);
				});
			cppIncludes.paths_.push_back("pix3.h");

			auto cppNamespaceObject = std::make_shared<Namespace>("OksEngine");

			{

				for (Common::Index i = 0; i < threads.size(); ++i) {
					auto& thread = threads[i];

					Variable::CreateInfo threadVariable{
						.type_ = "std::thread",
						.name_ = "thread" + std::to_string(i)
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadVariable));

					std::mutex thread16Mutex;

					Variable::CreateInfo threadMutexVariable{
						.type_ = "std::mutex",
						.name_ = "thread" + std::to_string(i) + "Mutex"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadMutexVariable));


					Variable::CreateInfo runSystemThreadVariable{
						.type_ = "std::atomic_bool",
						.name_ = "runSystemThread" + std::to_string(i),
						.initValue_ = "false"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(runSystemThreadVariable));

					Variable::CreateInfo threadCVVariable{
						.type_ = "std::condition_variable",
						.name_ = "thread" + std::to_string(i) + "CV"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
				}

				Variable::CreateInfo threadCVVariable{
						.type_ = "Common::UInt64",
						.name_ = "frameNumber",
						.initValue_ = "0"
				};
				cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
			}

			Code cppRunSystemsCode;
			{
				cppRunSystemsCode.Add(
					"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Frame %d\", frameNumber);"
					"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
					"world2->StartFrame();"
					"PIXEndEvent();");

				//Start threads.
				for (Common::Index i = 0; i < threads.size(); ++i) {
					auto& thread = threads[i];
					std::string threadId = std::to_string(i);
					cppRunSystemsCode.Add(std::format(
						"runSystemThread{} = true;"
						"{{"
						"std::unique_lock lock{{thread{}Mutex}};"
						"thread{}CV.notify_one();"
						"}}",
						threadId,
						threadId,
						threadId
					).c_str());
				}

				//Generate code to run main thread systems.
				for (Common::Index i = 0; i < mainThreads.size(); ++i) {
					auto& mainThread = mainThreads[i];
					Code runMainThreadSystems;
					for (auto it = mainThread.systems_.begin(); it != mainThread.systems_.end(); it++) {
						runMainThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							it->first,
							it->first
						));
						auto itCopy = it;
						if (itCopy++ != mainThread.systems_.end()) {
							runMainThreadSystems.NewLine();
						}
					}
					cppRunSystemsCode.Add(runMainThreadSystems);
				}

				//Wait for threads end.
				for (Common::Index i = 0; i < threads.size(); ++i) {
					auto& thread = threads[i];
					std::string threadId = std::to_string(i);
					cppRunSystemsCode.Add(std::format(
						"{{"
						"std::unique_lock lock{{thread{}Mutex}};"
						"thread{}CV.wait(lock, [&]() {{ return runSystemThread"
						"{} == false;"
						"}});"
						"}}",
						threadId,
						threadId,
						threadId
					));

			}

			//Generate code to run systems that process all entities.
			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
					if (systemEcsFile->ci_.processAllEntities_) {
						cppRunSystemsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + systemEcsFile->GetName() + "\");");
						cppRunSystemsCode.Add(systemEcsFile->GetName() + "System(world2);");
						cppRunSystemsCode.NewLine();
						cppRunSystemsCode.Add("PIXEndEvent();");
					}
				});

			cppRunSystemsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"EndFrame\");");
			cppRunSystemsCode.Add("world2->EndFrame();");
			cppRunSystemsCode.Add("PIXEndEvent();");
			cppRunSystemsCode.Add("PIXEndEvent();");
			cppRunSystemsCode.Add("++frameNumber;");
		}

		//CreateThreads method realization.
		Function::CreateInfo cppRunSystemsFunction{
			.name_ = "RunSystems",
			.parameters_ = {
				{ "std::shared_ptr<ECS2::World>", "world2" }
			},
			.returnType_ = "void",
			.code_ = cppRunSystemsCode,
			.isPrototype_ = false,
			.inlineModifier_ = false
		};

		auto cppRunSystemsFunctionObject = std::make_shared<Function>(cppRunSystemsFunction);
		cppNamespaceObject->Add(cppRunSystemsFunctionObject);


		//Create threads
		Code cppCreateThreadsCode;
		{

			for (Common::Index i = 0; i < threads.size(); ++i) {
				auto& thread = threads[i];
				cppCreateThreadsCode.Add("//Thread " + std::to_string(i));
				cppCreateThreadsCode.NewLine();
				//Add comment: what are components in use.
				std::set<std::string> threadComponents;
				for (auto& system : thread.systems_) {
					for (auto& component : system.second) {
						threadComponents.insert(component);
					}
				}
				for (auto& threadComponent : threadComponents) {
					cppCreateThreadsCode.Add("//" + threadComponent);
					cppCreateThreadsCode.NewLine();
				}

				cppCreateThreadsCode.Add("thread" + std::to_string(i) + " = std::thread{ [&](){ ");
				cppCreateThreadsCode.Add("while(true) { ");
				cppCreateThreadsCode.Add("std::unique_lock lock{ thread" + std::to_string(i) + "Mutex }; ");
				cppCreateThreadsCode.Add("thread" + std::to_string(i) + "CV.wait(lock, []() { return runSystemThread" + std::to_string(i) + "== true; });");
				cppCreateThreadsCode.Add("HRESULT r;");
				cppCreateThreadsCode.Add("r = SetThreadDescription(");
				cppCreateThreadsCode.Add("GetCurrentThread(),");
				cppCreateThreadsCode.Add("L\"Thread " + std::to_string(i) + "\"");
				cppCreateThreadsCode.Add(");");

				cppCreateThreadsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Thread " + std::to_string(i) + "\");");

				Code runThreadSystems;
				for (auto it = thread.systems_.begin(); it != thread.systems_.end(); it++) {
					runThreadSystems.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + it->first + "\");");
					runThreadSystems.Add(it->first + "System(world2);");
					runThreadSystems.NewLine();
					runThreadSystems.Add("PIXEndEvent();");
					auto itCopy = it;
					if (itCopy++ != thread.systems_.end()) {
						runThreadSystems.NewLine();
					}
				}

				runThreadSystems.ApplyTab();
				cppCreateThreadsCode.Add(runThreadSystems);
				cppCreateThreadsCode.Add("PIXEndEvent();");
				cppCreateThreadsCode.Add("runSystemThread" + std::to_string(i) + " = false;");
				cppCreateThreadsCode.Add("thread" + std::to_string(i) + "CV.notify_all();");
				cppCreateThreadsCode.Add("}");
				cppCreateThreadsCode.Add("} };");
			}

		}

		//CreateThreads method realization.
		Function::CreateInfo cppCreateThreadsFunction{
			.name_ = "CreateThreads",
			.parameters_ = {
				{ "std::shared_ptr<ECS2::World>", "world2" }
			},
			.returnType_ = "void",
			.code_ = cppCreateThreadsCode,
			.isPrototype_ = false,
			.inlineModifier_ = false
		};

		auto cppCreateThreadsFunctionObject = std::make_shared<Function>(cppCreateThreadsFunction);
		cppNamespaceObject->Add(cppCreateThreadsFunctionObject);


		//cpp file
		File::CreateInfo cppfci{
		.isHpp_ = false,
		.includes_ = cppIncludes,
		.base_ = cppNamespaceObject
		};

		auto cppFile = std::make_shared<File>(cppfci);


		//Generate .HPP.
		//Update method prototype.
		Function::CreateInfo hppRunSystemsFunction{
			.name_ = "RunSystems",
			.parameters_ = {
				{ "std::shared_ptr<ECS2::World>", "world2"}},
			.returnType_ = "void",
			.code_ = {},
			.isPrototype_ = true,
			.inlineModifier_ = false
		};

		auto hppRunSystemsFunctionObject = std::make_shared<Function>(hppRunSystemsFunction);

		//Create threads method prototype.
		Function::CreateInfo hppCreateThreadsFunction{
			.name_ = "CreateThreads",
			.parameters_ = { { "std::shared_ptr<ECS2::World>", "world2"} },
			.returnType_ = "void",
			.code_ = {},
			.isPrototype_ = true,
			.inlineModifier_ = false
		};

		auto hppCreateThreadsFunctionObject = std::make_shared<Function>(hppCreateThreadsFunction);

		auto hppNamespaceObject = std::make_shared<Namespace>("OksEngine");

		hppNamespaceObject->Add(hppCreateThreadsFunctionObject);
		hppNamespaceObject->Add(hppRunSystemsFunctionObject);

		File::Includes hppIncludes{ };
		hppIncludes.paths_.push_back("boost/asio/thread_pool.hpp");
		hppIncludes.paths_.push_back("boost/asio/post.hpp");
		hppIncludes.paths_.push_back("ECS2.World.hpp");


		//hpp file
		File::CreateInfo hppfci{
		.isHpp_ = true,
		.includes_ = hppIncludes,
		.base_ = hppNamespaceObject
		};

		auto hppFile = std::make_shared<File>(hppfci);


		//Generate files Paths

		auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

		std::filesystem::path includeDirFullPath;

		std::filesystem::path::iterator includeDirIt;
		for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
			auto folder = *it;
			if (folder == projectContext->includeDirectory_) {
				includeDirIt = it;
				break;
			}
		}

		for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
			includeDirFullPath /= *it;
		}

		includeDirFullPath /= *includeDirIt;

		std::filesystem::path systemHppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.hpp");
		FormatPath(systemHppFileFullPath);

		std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.cpp");
		FormatPath(systemCppFileFullPath);

		return {
			{ systemCppFileFullPath , cppFile },
			{ systemHppFileFullPath , hppFile }
		};
	}


	std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext) {

		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;


		ComponentFileStructureGenerator::CreateInfo ci{
			.includeDirectory_ = projectContext->includeDirectory_
		};

		projectContext->ForEachComponentEcsFile(
			[&](std::shared_ptr<ParsedComponentECSFile> componentEcsFile) {

				ComponentFileStructureGenerator generator{ ci };
				auto file = generator.GenerateECSCXXFilesStructure(projectContext, componentEcsFile);
				files.push_back(file);
			});

		projectContext->ForEachSystemEcsFile(
			[&](std::shared_ptr<ParsedSystemECSFile> componentEcsFile) {
				SystemFileStructureGenerator::CreateInfo ci{
					.includeDirectory_ = projectContext->includeDirectory_
				};
				SystemFileStructureGenerator generator{ ci };
				auto file = generator.GenerateECSCXXFilesStructure(projectContext, componentEcsFile);



				files.push_back(file);
			});



		//Generate OksEngine.Components.hpp files For COMPONENTS.
		for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

			//Generate components includes for components in the subdirectory 
			File::Includes includes{};
			std::filesystem::path componentIncludesFilePath;
			for (auto componentParsedEcsFile : componentsSystems.components_) {
				/*std::filesystem::path componentFullPath = ;
				componentIncludesFilePath = componentFullPath.parent_path();*/
				//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(componentFullPath.parent_path(), projectContext->includeDirectory_);
				const std::filesystem::path includePath = GetSubPath(
					componentParsedEcsFile->GetPath().parent_path(),
					projectContext->includeDirectory_,
					ResultRange::FromStartFolderToEnd,
					SearchDirection::FromEndToBegin,
					true);
				//componentIncludePath /= ;
				includes.paths_.push_back(includePath / ("auto_OksEngine." + componentParsedEcsFile->GetName() + ".hpp"));
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
						//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
						const std::filesystem::path componentIncludePath = GetSubPath(
							subdirPath,
							projectContext->includeDirectory_,
							ResultRange::FromStartFolderToEnd,
							SearchDirection::FromEndToBegin,
							true);
						//componentIncludePath ;
						includes.paths_.push_back(componentIncludePath / ("auto_OksEngine.Components.hpp"));
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
				//std::filesystem::path systemFullPath = 
				//systemsIncludesFilePath = systemFullPath.parent_path();
				//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(systemFullPath.parent_path(), projectContext->includeDirectory_);

				const std::filesystem::path systemIncludePath = GetSubPath(
					systemParsedEcsFile->GetPath().parent_path(),
					projectContext->includeDirectory_,
					ResultRange::FromStartFolderToEnd,
					SearchDirection::FromEndToBegin,
					true);

				//systemIncludePath /= ;
				includes.paths_.push_back(systemIncludePath / ("auto_OksEngine." + systemParsedEcsFile->GetName() + ".hpp"));
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
						//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
						const std::filesystem::path systemIncludePath = GetSubPath(
							subdirPath,
							projectContext->includeDirectory_,
							ResultRange::FromStartFolderToEnd,
							SearchDirection::FromEndToBegin,
							true);
						//systemIncludePath ;
						includes.paths_.push_back(systemIncludePath / ("auto_OksEngine.Systems.hpp"));
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

		return code;
	}

	Code GenerateECSCXXFilesStructure(std::shared_ptr<Struct> structObject) {
		Code code;
		code.Add("struct " + structObject->GetName() + " ");
		if (structObject->HasParent()) {
			code.Add(": " + structObject->GetParentName() + " ");
		}
		code.Add("{");



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
				code.Add(GenerateECSCXXFilesStructure(functionObject));
				structRealization.Add(code);
			}
			structRealization.ApplyTab();
			code.Add(structRealization);
		}
		code.Add("};");



		return code;
	}

	Code GenerateECSCXXFilesStructure(std::shared_ptr<Namespace> namespaceObject) {
		Code code;

		code.Add("namespace " + namespaceObject->GetName() + " {");


		for (auto base : namespaceObject->base_) {
			Code namespaceCode = GenerateECSCXXFilesStructure(base);
			namespaceCode.ApplyTab();
			code.Add(namespaceCode);
		}

		code.Add("}");

		return code;
	}

	Code GenerateECSCXXFilesStructure(std::shared_ptr<Variable> variableObject) {
		Code code;
		code.Add(
			variableObject->ci_.type_ +
			" " +
			variableObject->ci_.name_ +
			"{" + variableObject->ci_.initValue_ + "};");
		return code;
	}

	Code GenerateECSCXXFilesStructure(std::shared_ptr<Function> functionObject) {
		Code code;

		if (!functionObject->ci_.templateParameters_.empty()) {
			code.Add("template<");
			for (Common::Index i = 0; i < functionObject->ci_.templateParameters_.size(); ++i) {
				code.Add("class " + functionObject->ci_.templateParameters_[i]);
				if (i != functionObject->ci_.templateParameters_.size() - 1) {
					code.Add(", ");
				}
			}
			code.Add(">");

		}

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

			Code realization = functionObject->ci_.code_;
			realization.ApplyTab();
			code.Add(realization);

			code.Add("};");

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

			return code;
		}
		return code;
	}

	Code GenerateECSCXXFilesStructure(std::shared_ptr<Base> base) {
		Code code;
		if (base == nullptr) {
			return code;
		}
		if (base->GetType() == Base::Type::Namespace) {
			auto namespaceObject = std::dynamic_pointer_cast<Namespace>(base);
			code = GenerateECSCXXFilesStructure(namespaceObject);

		}
		else if (base->GetType() == Base::Type::Struct) {
			auto structObject = std::dynamic_pointer_cast<Struct>(base);
			code = GenerateECSCXXFilesStructure(structObject);
		}
		else if (base->GetType() == Base::Type::Function) {
			auto functionObject = std::dynamic_pointer_cast<Function>(base);
			code = GenerateECSCXXFilesStructure(functionObject);
		}
		else if (base->GetType() == Base::Type::Variable) {
			auto functionObject = std::dynamic_pointer_cast<Variable>(base);
			code = GenerateECSCXXFilesStructure(functionObject);
		}
		clang::format::FormatStyle style = clang::format::getGoogleStyle(clang::format::FormatStyle::LanguageKind::LK_Cpp);
		llvm::StringRef codeToFormat = code.code_.c_str();  // ������ ������ ����

		// ���������� �������� ��� ����� ����
		clang::tooling::Replacements replacements = clang::format::reformat(style, codeToFormat, { clang::tooling::Range(0, codeToFormat.size()) });

		auto formattedCode = clang::tooling::applyAllReplacements(codeToFormat, replacements);
		if (!formattedCode) {
			llvm::errs() << "Error formatting the code.\n";
		}
		code.code_ = *formattedCode;
		return code;
	}

	std::shared_ptr<CodeFile> GenerateCode(std::shared_ptr<File> fileStructure) {
		return GenerateECSCXXFilesStructure(fileStructure);
	}

	std::shared_ptr<CodeFile> GenerateECSCXXFilesStructure(std::shared_ptr<File> fileStructure) {

		auto codeFile = std::make_shared<CodeFile>();

		if (fileStructure->IsHpp()) {
			Code code;
			code.Add("#pragma once");
			code.NewLine();

			codeFile->AddCode(code);
		}
		if (fileStructure->createInfo_.includes_.paths_.size() == 0) {
			//__debugbreak();
		}
		//Includes.
		{
			Code includes;
			fileStructure->ForEachInclude([&](const std::filesystem::path& path) {
				includes.Add(GenerateInclude(path));
				includes.NewLine();
				});
			includes.NewLine();
			codeFile->AddCode(includes);
		}
		const Code code = GenerateECSCXXFilesStructure(fileStructure->GetBase());
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