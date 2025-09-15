#pragma once 

#include <ECSGenerator.Code.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.ParsedSystemECSFile.hpp>
#include <ECSGenerator.ParsedComponentECSFile.hpp>

namespace ECSGenerator {




	//class SystemFileStructureGenerator {
	//public:

	//	struct CreateInfo {
	//		std::string includeDirectory_;
	//	};

	//	SystemFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

	//	std::pair<
	//		std::filesystem::path,
	//		std::shared_ptr<File>> GenerateSystemRealization(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


	//		File::Includes includes{ };


	//		const std::string systemName = systemEcsFile->GetName();
	//		std::filesystem::path systemsIncludesFilePath;
	//		std::filesystem::path systemFullPath = systemEcsFile->GetPath();
	//		systemsIncludesFilePath = systemFullPath.parent_path();
	//		std::filesystem::path systemIncludePath = GetSubPath(
	//			systemFullPath.parent_path(),
	//			projectContext->includeDirectory_,
	//			ResultRange::FromStartFolderToEnd,
	//			SearchDirection::FromEndToBegin,
	//			true);

	//		systemIncludePath /= ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
	//		includes.paths_.push_back(systemIncludePath);


	//		auto namespaceObject = std::make_shared<Namespace>("OksEngine");
	//		{
	//			using namespace std::string_literals;
	//			//Update method.
	//			std::vector<Function::Parameter> updateMethodParameters;


	//			//If system  process all entities need only one paramter Entity::Id
	//			if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {
	//				updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
	//			}


	//			for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

	//				std::string entityIdString = "";
	//				if (systemEcsFile->ci_.processesEntities_.size() == 2) {
	//					entityIdString = (i == 0) ? ("1") : ("2");
	//				}

	//				updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
	//				const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.processesEntities_[i];
	//				entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
	//					updateMethodParameters.push_back({
	//						((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
	//						lowerIncludeName });

	//					return true;
	//					});
	//			}

	//			Function::CreateInfo updateMethodCI{
	//				.name_ = systemEcsFile->GetName() + "::Update",
	//				.parameters_ = updateMethodParameters,
	//				.returnType_ = "void",
	//				.code_ = "",
	//				.isPrototype_ = false,
	//				.inlineModifier_ = false
	//			};

	//			auto updateMethod = std::make_shared<Function>(updateMethodCI);

	//			namespaceObject->Add(updateMethod);
	//		}

	//		File::CreateInfo fci{
	//			.isHpp_ = false,
	//			.includes_ = includes,
	//			.base_ = namespaceObject
	//		};
	//		auto file = std::make_shared<File>(fci);

	//		std::filesystem::path componentHppFileFullPath
	//			= systemEcsFile->GetPath().parent_path() / ("OksEngine." + systemEcsFile->GetName() + ".cpp");
	//		std::string componentHppFileFullPathString = componentHppFileFullPath.string();
	//		std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
	//		return { std::filesystem::path{ componentHppFileFullPathString }, file };

	//	}


	//	std::pair<
	//		std::filesystem::path,
	//		std::shared_ptr<File>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

	//		// // ������� CompilerInstance
	//		// clang::CompilerInstance CI;
	//		// CI.createDiagnostics();
	//		// CI.getLangOpts().CPlusPlus = true; // �������� ��������� C++

	//		// // ������� ASTContext
	//		// CI.createASTContext();

	//		// // ������� Rewriter ��� ��������� ����
	//		// clang::Rewriter R;
	//		// R.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());

	//		// // ������� TranslationUnitDecl (�������� ���� AST)
	//		// clang::ASTContext& AST = CI.getASTContext();
	//		// clang::TranslationUnitDecl* TU = AST.getTranslationUnitDecl();

	//		// // ��������� ��������� �������������
	//		// R.InsertText(clang::SourceLocation(), "#pragma once\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <Common.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <ECS2.Component.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <ECS2.Entity.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <ECS2.World.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <Lua.Context.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <glm/glm.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <glm/gtc/quaternion.hpp>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <imgui.h>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <string>\n");
	//		// R.InsertText(clang::SourceLocation(), "#include <vector>\n\n");

	//		// // ������� ������������ ���� OksEngine
	//		// clang::NamespaceDecl* OksEngine = clang::NamespaceDecl::Create(
	//		// 	AST, TU, false, clang::SourceLocation(), clang::SourceLocation(),
	//		// 	&AST.Idents.get("OksEngine"), nullptr, false);

	//		// // ������� ��������� Animation
	//		// clang::RecordDecl* Animation = clang::CXXRecordDecl::Create(
	//		// 	AST, clang::TagDecl::TagKind::Struct, OksEngine, clang::SourceLocation(),
	//		// 	clang::SourceLocation(), &AST.Idents.get("Animation"));

	//		// // ��������� ������� ����� ECS2::IComponent<Animation>
	//		// clang::QualType BaseType = AST.getTemplateSpecializationTypeInfo(
	//		// 	AST.getRecordType(AST.getRecordType(AST.getTypeDeclType(
	//		// 		AST.getTypeDecl(TU, clang::SourceLocation(), &AST.Idents.get("ECS2")))),
	//		// 		{ AST.getTypeDeclType(Animation) });
	//		// clang::CXXBaseSpecifier * BaseSpec = clang::CXXBaseSpecifier::Create(
	//		// 	AST, clang::SourceLocation(), false, false, BaseType, clang::SourceLocation(),
	//		// 	clang::SourceLocation());
	//		// Animation->setBases(BaseSpec, 1);

	//		// // ��������� ����������� �� ���������
	//		// clang::CXXConstructorDecl * DefaultCtor = clang::CXXConstructorDecl::Create(
	//		// 	AST, Animation, clang::SourceLocation(), clang::DeclarationName(), clang::QualType(),
	//		// 	nullptr, clang::Function::FunctionKind::FK_DefaultConstructor, false, false,
	//		// 	false, false);
	//		// DefaultCtor->setAccess(clang::AccessSpecifier::AS_public);
	//		// Animation->addDecl(DefaultCtor);

	//		// // ��������� ����
	//		// clang::FieldDecl * NameField = clang::FieldDecl::Create(
	//		// 	AST, Animation, SourceLocation(), SourceLocation(),
	//		// 	&AST.Idents.get("name_"), AST.getStdStringType(), nullptr, nullptr,
	//		// 	false, false);
	//		// NameField->setAccess(clang::AccessSpecifier::AS_private);
	//		// Animation->addDecl(NameField);

	//		// clang::FieldDecl * DurationField = clang::FieldDecl::Create(
	//		// 	AST, Animation, SourceLocation(), SourceLocation(),
	//		// 	&AST.Idents.get("durationInTicks_"), AST.getDoubleType(), nullptr,
	//		// 	nullptr, false, false);
	//		// DurationField->setAccess(clang::AccessSpecifier::AS_private);
	//		// Animation->addDecl(DurationField);

	//		// // ��������� ����������� � �����������
	//		// QualType ParamTypes[] = {
	//		// 	AST.getStdStringType(),
	//		// 	AST.getDoubleType(),
	//		// 	AST.FloatTy,
	//		// 	AST.getTemplateSpecializationType(
	//		// 		AST.getRecordType(
	//		// 			AST.getRecordDecl(
	//		// 				AST.getTypeDeclType(
	//		// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
	//		// 				)
	//		// 			)
	//		// 		)
	//		// 	),
	//		// 	AST.getTemplateSpecializationType(
	//		// 		AST.getRecordType(
	//		// 			AST.getRecordDecl(
	//		// 				AST.getTypeDeclType(
	//		// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
	//		// 				)
	//		// 			)
	//		// 		)
	//		// 	),
	//		// 	AST.getTemplateSpecializationType(
	//		// 		AST.getRecordType(
	//		// 			AST.getRecordDecl(
	//		// 				AST.getTypeDeclType(
	//		// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
	//		// 				)
	//		// 			)
	//		// 		)
	//		// 	)
	//		// };
	//		// CXXConstructorDecl * ParamCtor = CXXConstructorDecl::Create(
	//		// 	AST, Animation, SourceLocation(), DeclarationName(), QualType(),
	//		// 	nullptr, Function::FunctionKind::FK_Constructor, false, false, false,
	//		// 	false);
	//		// ParamCtor->setAccess(AccessSpecifier::AS_public);
	//		// Animation->addDecl(ParamCtor);

	//		// // ��������� ����� GetName
	//		// FunctionDecl * GetNameMethod = FunctionDecl::Create(
	//		// 	AST, Animation, SourceLocation(), SourceLocation(),
	//		// 	DeclarationName(&AST.Idents.get("GetName")), AST.getPointerType(AST.CharTy),
	//		// 	nullptr, Function::FunctionKind::FK_MemberFunction, false, false, false,
	//		// 	false);
	//		// GetNameMethod->setAccess(AccessSpecifier::AS_public);
	//		// Animation->addDecl(GetNameMethod);

	//		// // ���������� ���
	//		// Animation->print(R.getEditBuffer(CI.getSourceManager().getMainFileID()));

	//		// // ������� ��������������� ���
	//		// R.getEditBuffer(CI.getSourceManager().getMainFileID()).write(llvm::outs());

	//		File::Includes includes{ };
	//		includes.paths_.push_back("ECS2.hpp");
	//		includes.paths_.push_back("chrono");
	//		if (!systemEcsFile->ci_.createEntityComponents_.empty()) {
	//			for (auto& createEntityComponent : systemEcsFile->ci_.createEntityComponents_) {

	//				auto componentEcsFile = projectContext->GetEcsFileByName(createEntityComponent);

	//				const std::filesystem::path includePath = GetSubPath(
	//					componentEcsFile->GetPath().parent_path(),
	//					projectContext->includeDirectory_,
	//					ResultRange::FromStartFolderToEnd,
	//					SearchDirection::FromEndToBegin,
	//					true);
	//				includes.paths_.push_back(includePath / ("auto_OksEngine." + createEntityComponent + ".hpp"));
	//			}
	//		}

	//		for (auto& entity : systemEcsFile->ci_.processesEntities_) {

	//			//Generate includes for components that system uses as input.
	//			for (auto componentInclude : entity.includes_) {
	//				auto componentEcsFile = projectContext->GetEcsFileByName(componentInclude.name_);
	//				const std::filesystem::path includePath = GetSubPath(
	//					componentEcsFile->GetPath().parent_path(),
	//					projectContext->includeDirectory_,
	//					ResultRange::FromStartFolderToEnd,
	//					SearchDirection::FromEndToBegin,
	//					true);

	//				includes.paths_.push_back(includePath / ("auto_OksEngine." + componentInclude.name_ + ".hpp"));
	//			}

	//			//Generate includes for components that system creates.
	//			for (auto componentCreates : entity.creates_) {
	//				auto componentEcsFile = projectContext->GetEcsFileByName(componentCreates);
	//				const std::filesystem::path includePath = GetSubPath(
	//					componentEcsFile->GetPath().parent_path(),
	//					projectContext->includeDirectory_,
	//					ResultRange::FromStartFolderToEnd,
	//					SearchDirection::FromEndToBegin,
	//					true);
	//				includes.paths_.push_back(includePath / ("auto_OksEngine." + componentCreates + ".hpp"));
	//			}

	//			//Generate includes for components that system creates.
	//			for (auto componentExclude : entity.excludes_) {
	//				auto componentEcsFile = projectContext->GetEcsFileByName(componentExclude);
	//				const std::filesystem::path includePath = GetSubPath(
	//					componentEcsFile->GetPath().parent_path(),
	//					projectContext->includeDirectory_,
	//					ResultRange::FromStartFolderToEnd,
	//					SearchDirection::FromEndToBegin,
	//					true);
	//				includes.paths_.push_back(includePath / ("auto_OksEngine." + componentExclude + ".hpp"));
	//			}

	//		}
	//		auto namespaceObject = std::make_shared<Namespace>("OksEngine");

	//		using namespace std::string_literals;
	//		//Update method.
	//		std::vector<Function::Parameter> updateMethodParameters;

	//		//If system  process all entities need only one paramter Entity::Id
	//		if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {
	//			updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
	//		}

	//		for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

	//			std::string entityIdString = "";
	//			if (systemEcsFile->ci_.processesEntities_.size() == 2) {
	//				entityIdString = (i == 0) ? ("1") : ("2");
	//			}

	//			updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
	//			const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.processesEntities_[i];
	//			entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//				std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
	//				updateMethodParameters.push_back({
	//					((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
	//					lowerIncludeName });

	//				return true;
	//				});
	//		}

	//		//Add entity index to component names if there are two components with the same name.



	//		Function::CreateInfo updateMethodCI{
	//			.name_ = "Update",
	//			.parameters_ = updateMethodParameters,
	//			.returnType_ = "void",
	//			.code_ = "",
	//			.isPrototype_ = true,
	//			.inlineModifier_ = false
	//		};

	//		auto updateMethod = std::make_shared<Function>(updateMethodCI);

	//		//CreateEntity method.
	//		Function::CreateInfo createDynamicEntityCI{
	//			.name_ = "CreateEntity",
	//			.parameters_ = { },
	//			.returnType_ = "ECS2::Entity::Id",
	//			.code_ = "return world_->CreateEntity();",
	//			.isPrototype_ = false,
	//			.inlineModifier_ = false
	//		};

	//		auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

	//		//CreateEntity<> method.
	//		Function::CreateInfo createArchetypeEntityCI{
	//			.name_ = "CreateEntity",
	//			.parameters_ = { },
	//			.returnType_ = "ECS2::Entity::Id",
	//			.code_ = "return world_->CreateEntity<Components...>();",
	//			.isPrototype_ = false,
	//			.inlineModifier_ = false,
	//			.templateParameters_ = { "...Components" }
	//		};

	//		auto createArchetypeEntityMethod = std::make_shared<Function>(createArchetypeEntityCI);

	//		//CreateComponent method.
	//		Function::CreateInfo createComponentCI{
	//			.name_ = "CreateComponent",
	//			.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
	//			.returnType_ = "void",
	//			.code_ = "world_->CreateComponent<Component>(entityId, std::forward<Args>(args)...);",
	//			.isPrototype_ = false,
	//			.inlineModifier_ = false,
	//			.templateParameters_ = { "Component", "...Args" }
	//		};

	//		auto createComponentMethod = std::make_shared<Function>(createComponentCI);


	//		//GetComponent method.
	//		Code getComponentCode;
	//		{
	//			if (!systemEcsFile->ci_.accessEntityComponents_.empty()) {
	//				getComponentCode.Add("ASSERT_FMSG(");
	//				systemEcsFile->ForEachAccessComponent(
	//					[&](const std::string& systemName, bool isLast) {
	//						getComponentCode.Add(std::format("{} == Component::GetName()", "\"" + systemName + "\""));
	//						if (!isLast) {
	//							getComponentCode.Add(" || ");
	//						}
	//						return true;
	//					});
	//				getComponentCode.Add(", \"Attempt to access component that system(" + systemEcsFile->GetName() + ") can't access. Added access entities description to .ecs file that corresponds to system\");");
	//			}
	//			getComponentCode.Add("return world_->GetComponent<Component>(entityId);");
	//		}
	//		Function::CreateInfo getComponentCI{
	//			.name_ = "GetComponent",
	//			.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
	//			.returnType_ = "Component*",
	//			.code_ = getComponentCode,
	//			.isPrototype_ = false,
	//			.inlineModifier_ = false,
	//			.templateParameters_ = { "Component" }
	//		};

	//		auto getComponentMethod = std::make_shared<Function>(getComponentCI);

	//		//IsComponentExist method.
	//		Function::CreateInfo isComponentExistCI{
	//			.name_ = "IsComponentExist",
	//			.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
	//			.returnType_ = "bool",
	//			.code_ = "return world_->IsComponentExist<Component>(entityId);",
	//			.isPrototype_ = false,
	//			.inlineModifier_ = false,
	//			.templateParameters_ = { "Component" }
	//		};

	//		auto isComponentExistMethod = std::make_shared<Function>(isComponentExistCI);

	//		Struct::CreateInfo sci{
	//			.name_ = systemEcsFile->GetName(),
	//			.parent_ = "",
	//			.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
	//			.methods_ = { updateMethod, createComponentMethod, getComponentMethod, isComponentExistMethod, createDynamicEntityMethod, createArchetypeEntityMethod }
	//		};
	//		auto structObject = std::make_shared<Struct>(sci);



	//		namespaceObject->Add(structObject);

	//		std::vector<Function::Parameter> parameters;
	//		parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

	//		Code realization;
	//		if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::OneEntity ||
	//			systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::TwoEntities) {

	//			//Render render{ world };
	//			realization.Add(std::format(
	//				"{} {}{{ world }};",
	//				systemEcsFile->GetName(),
	//				systemEcsFile->GetLowerName()
	//			));

	//			realization.NewLine();

	//			//Excludes
	//			realization.Add("ECS2::ComponentsFilter excludeEntity1;");
	//			realization.NewLine();
	//			if ((systemEcsFile->GetSystemType() == ParsedSystemECSFile::SystemType::OneEntity ||
	//				systemEcsFile->GetSystemType() == ParsedSystemECSFile::SystemType::TwoEntities)
	//				&& !systemEcsFile->ci_.processesEntities_[0].excludes_.empty()) {
	//				realization.Add("excludeEntity1.SetBits<");
	//			}


	//			for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_[0].excludes_.size(); ++i) {
	//				realization.Add(systemEcsFile->ci_.processesEntities_[0].excludes_[i]);
	//				if (i != systemEcsFile->ci_.processesEntities_[0].excludes_.size() - 1) {
	//					realization.Add(", ");
	//				}

	//			}
	//			if (!systemEcsFile->ci_.processesEntities_[0].excludes_.empty()) {
	//				realization.Add(">();");
	//				realization.NewLine();
	//			}

	//			realization.Add("world->ForEachEntity<");
	//			realization.NewLine();
	//			systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//				realization.Add(include.name_);
	//				if (!isLast) {
	//					realization.Add(", ");
	//				}
	//				realization.NewLine();
	//				return true;
	//				});
	//			realization.Add(">(excludeEntity1, [&](");
	//			realization.Add("ECS2::Entity::Id entity1Id, ");
	//			systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//				realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
	//				if (!isLast) {
	//					realization.Add(", ");
	//				}
	//				realization.NewLine();
	//				return true;
	//				});
	//			realization.Add("){");
	//			realization.NewLine();

	//			if (systemEcsFile->ci_.processesEntities_.size() == 1) {
	//				//Call update method
	//				realization.Add(std::string{ (char)std::tolower(systemEcsFile->GetName()[0]) } + systemEcsFile->GetName().substr(1) + ".Update(");
	//				realization.Add("entity1Id");
	//				realization.Add(", ");
	//				realization.NewLine();
	//				systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
	//					if (!isLast) {
	//						realization.Add(", ");
	//					}
	//					realization.NewLine();
	//					return true;
	//					});
	//				realization.Add(");");
	//				realization.NewLine();

	//			}
	//			else {
	//				//Excludes
	//				realization.Add("ECS2::ComponentsFilter excludeEntity2;");
	//				realization.NewLine();
	//				if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
	//					realization.Add("excludeEntity2.SetBits<");
	//				}
	//				for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_[1].excludes_.size(); ++i) {
	//					realization.Add(systemEcsFile->ci_.processesEntities_[1].excludes_[i]);
	//					if (i != systemEcsFile->ci_.processesEntities_[1].excludes_.size() - 1) {
	//						realization.Add(", ");
	//					}

	//				}
	//				if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
	//					realization.Add(">();");
	//					realization.NewLine();
	//				}

	//				realization.Add("world->ForEachEntity<");
	//				realization.NewLine();
	//				systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					realization.Add(include.name_);
	//					if (!isLast) {
	//						realization.Add(", ");

	//					}
	//					realization.NewLine();
	//					return true;
	//					});
	//				realization.Add(">(excludeEntity2, [&](");
	//				realization.Add("ECS2::Entity::Id entity2Id, ");
	//				realization.NewLine();
	//				systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
	//					if (!isLast) {
	//						realization.Add(", ");
	//					}
	//					return true;
	//					});
	//				realization.Add("){");
	//				realization.NewLine();

	//				//Call update method
	//				realization.Add(systemEcsFile->GetLowerName() + ".Update(");
	//				realization.Add("entity1Id");
	//				realization.Add(", ");
	//				realization.NewLine();
	//				systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
	//					if (!isLast) {
	//						realization.Add(", ");
	//					}
	//					return true;
	//					});

	//				realization.Add(", ");
	//				realization.Add("entity2Id");
	//				realization.Add(", ");
	//				systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
	//					realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
	//					if (!isLast) {
	//						realization.Add(", ");
	//					}
	//					return true;
	//					});


	//				realization.Add(");");
	//				realization.NewLine();
	//				realization.Add("});");
	//				realization.NewLine();
	//			}

	//			realization.Add("});");
	//			realization.NewLine();
	//		}
	//		else if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {
	//			realization.Add(systemEcsFile->GetName() + " " + systemEcsFile->GetLowerName() + "{ world };");
	//			realization.NewLine();
	//			realization.Add("world->ForEachEntity([&](ECS2::Entity::Id entityId){");
	//			realization.Add(systemEcsFile->GetLowerName() + ".Update(");
	//			realization.Add("entityId);");
	//			realization.Add("});");

	//		}
	//		else if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {

	//			realization.Add(std::format(
	//				"{} {}{{ world }};",
	//				systemEcsFile->GetName(),
	//				systemEcsFile->GetLowerName()
	//			));
	//			realization.Add(std::format(
	//				"{}.Update();",
	//				systemEcsFile->GetLowerName()
	//			));
	//		}

	//		Function::CreateInfo funcci{
	//			.name_ = systemEcsFile->GetName() + "System",
	//			.parameters_ = parameters,
	//			.returnType_ = "void",
	//			.code_ = realization,
	//			.inlineModifier_ = true

	//		};
	//		auto runSystem = std::make_shared<Function>(funcci);

	//		namespaceObject->Add(runSystem);


	//		File::CreateInfo fci{
	//			.isHpp_ = true,
	//			.includes_ = includes,
	//			.base_ = namespaceObject
	//		};
	//		auto file = std::make_shared<File>(fci);

	//		std::filesystem::path systemHppFileFullPath
	//			= systemEcsFile->GetPath().parent_path() / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
	//		std::string systemHppFileFullPathString = systemHppFileFullPath.string();
	//		std::replace(systemHppFileFullPathString.begin(), systemHppFileFullPathString.end(), '\\', '/');
	//		return { std::filesystem::path{ systemHppFileFullPathString }, file };


	//	}



	//	CreateInfo ci_;
	//};

 //   

	//class ComponentFileStructureGenerator {
	//public:

	//	struct CreateInfo {
	//		std::string includeDirectory_;
	//	};

	//	ComponentFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}



	//	std::pair<
	//		std::filesystem::path,
	//		std::shared_ptr<File>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedComponentECSFile> componentEcsFile) {

	//		auto namespaceObject = std::make_shared<Namespace>("OksEngine");

	//		{

	//			std::vector<Struct::Field> fields;

	//			componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast)->bool {

	//				Struct::Field field{
	//					.type_ = fieldInfo.GetTypeName(),
	//					.name_ = fieldInfo.GetName(),
	//					.copyable_ = fieldInfo.copyable_
	//				};

	//				fields.push_back(field);

	//				return true;
	//				});
	//			//
	//			//				// ������� �������� � ������
	//			//				llvm::LLVMContext Context;
	//			//				llvm::Module* Mod = new llvm::Module("main_module", Context);
	//			//
	//			//				// ������� ������� main
	//			//				llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
	//			//				llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", Mod);
	//			//
	//			//				// ������� ������� ����
	//			//				llvm::BasicBlock* BB = llvm::BasicBlock::Create(Context, "entry", F);
	//			//				llvm::IRBuilder<> Builder(BB);
	//			//
	//			//				// ���������� 42
	//			//				Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 42));
	//			//
	//			//				// ��������� ������������ ������
	//			//				verifyFunction(*F);
	//			//
	//			//				// ������� LLVM IR
	//			//				//Mod->print(outs(), nullptr);
	//			//
	//			//				/*[[nodiscard]]
	//			//				static const char* GetName() noexcept {
	//			//				return Common::TypeInfo<Type>::GetTypeName();
	//			//				}*/
	//			//
	//			//				{
	//			//					// ������� ��������� �����������
	//			//					clang::CompilerInstance CI;
	//			//					CI.createDiagnostics();
	//			//#pragma region Assert
	//			//					ASSERT_FMSG(CI.hasDiagnostics(), "");
	//			//#pragma endregion
	//			//					// ����������� LangOptions (��������� �����)
	//			//					clang::LangOptions LO;
	//			//					CI.getLangOpts() = LO;
	//			//					// ����������� ������� ������� (target triple)
	//			//					std::shared_ptr<clang::TargetOptions> TO = std::make_shared<clang::TargetOptions>();
	//			//					//TO->Triple = "x86_64-pc-windows-msvc";
	//			//					TO->Triple = llvm::sys::getDefaultTargetTriple(); // �������� ������� �� ���������
	//			//					CI.setTarget(clang::TargetInfo::CreateTargetInfo(CI.getDiagnostics(), TO));
	//			//					if (!CI.hasTarget()) {
	//			//						//llvm::errs() << "������: �� ������� ������� TargetInfo\n";
	//			//						//return 1;
	//			//					}
	//			//					// ����������� FileManager
	//			//					CI.createFileManager();
	//			//#pragma region Assert
	//			//					ASSERT_FMSG(CI.hasFileManager(), "");
	//			//#pragma endregion
	//			//					// ����������� SourceManager
	//			//					CI.createSourceManager(CI.getFileManager());
	//			//#pragma region Assert
	//			//					ASSERT_FMSG(CI.hasSourceManager(), "");
	//			//#pragma endregion
	//			//					// ����������� Preprocessor
	//			//					CI.createPreprocessor(clang::TranslationUnitKind::TU_Complete);
	//			//#pragma region Assert
	//			//					ASSERT_FMSG(CI.hasPreprocessor(), "������: �� ������� ������� Preprocessor.");
	//			//#pragma endregion
	//			//					// ������� ASTContext
	//			//					CI.createASTContext();
	//			//
	//			//					// ������� Rewriter ��� ��������� ����
	//			//					clang::Rewriter R;
	//			//					R.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
	//			//
	//			//					// ������� FunctionDecl ��� main()
	//			//					clang::FunctionDecl* FD = clang::FunctionDecl::Create(
	//			//						CI.getASTContext(),
	//			//						CI.getASTContext().getTranslationUnitDecl(),
	//			//						clang::SourceLocation(),
	//			//						clang::SourceLocation(),
	//			//						clang::DeclarationName(&CI.getASTContext().Idents.get("main")),
	//			//						CI.getASTContext().getFunctionType(
	//			//							CI.getASTContext().IntTy, // ������������ ��� (int)
	//			//							{},                       // ��������� (�����)
	//			//							clang::FunctionProtoType::ExtProtoInfo()
	//			//						),
	//			//						nullptr, // ��� (���� ����)
	//			//						clang::StorageClass::SC_None
	//			//					);
	//			//
	//			//					// ������� ���� ������� (return 42;)
	//			//					clang::ReturnStmt* RS = clang::ReturnStmt::Create(
	//			//						CI.getASTContext(),
	//			//						clang::SourceLocation(),
	//			//						clang::IntegerLiteral::Create(
	//			//							CI.getASTContext(),
	//			//							llvm::APInt(32, 42), // �������� 42
	//			//							CI.getASTContext().IntTy,
	//			//							clang::SourceLocation()
	//			//						),
	//			//						nullptr
	//			//					);
	//			//
	//			//					// ������� CompoundStmt (���� �������)
	//			//					clang::CompoundStmt* CS = clang::CompoundStmt::Create(
	//			//						CI.getASTContext(),
	//			//						clang::ArrayRef<clang::Stmt*>(RS),
	//			//						clang::FPOptionsOverride{},
	//			//						clang::SourceLocation(),
	//			//						clang::SourceLocation()
	//			//					);
	//			//
	//			//					// ����������� ���� �������
	//			//					FD->setBody(CS);
	//			//
	//			//					// ���������� C++ ��� �� ������ AST
	//			//					FD->print(llvm::outs());
	//			//
	//			//					// ������� ��������������� ���
	//			//					R.getEditBuffer(CI.getSourceManager().getMainFileID()).write(llvm::outs());
	//			//
	//			//				

	//			Function::CreateInfo fci{
	//				.name_ = "GetName",
	//				.parameters_ = {},
	//				.returnType_ = "const char*",
	//				.code_ = "return Common::TypeInfo<" + componentEcsFile->GetName() + ">::GetTypeName();",
	//				.inlineModifier_ = false,
	//				.staticModifier_ = true
	//			};

	//			auto getNameMethod = std::make_shared<Function>(fci);

	//			Struct::CreateInfo sci{
	//				.name_ = componentEcsFile->GetName(),
	//				.parent_ = "ECS2::IComponent<" + componentEcsFile->GetName() + ">",
	//				.fields_ = fields,
	//				.methods_ = { getNameMethod }
	//			};
	//			auto structObject = std::make_shared<Struct>(sci);



	//			namespaceObject->Add(structObject);
	//		}

	//		//Edit function.
	//		{
	//			Code realization;
	//			componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
	//				if (IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
	//					realization.Add(GenerateTypeImGuiEditCode(
	//						componentEcsFile->GetLowerName(),
	//						fieldInfo.GetTypeName(),
	//						fieldInfo.GetName(),
	//						fieldInfo.GetName() + "_")
	//						.NewLine());
	//				}
	//				return true;
	//				});

	//			Function::CreateInfo fci{
	//				.name_ = "Edit" + componentEcsFile->GetName(),
	//				.parameters_ = {
	//					{ componentEcsFile->GetName() + "*", componentEcsFile->GetLowerName()}
	//				},
	//				.returnType_ = "void",
	//				.code_ = realization,
	//				.inlineModifier_ = true
	//			};

	//			auto editFunction = std::make_shared<Function>(fci);

	//			namespaceObject->Add(editFunction);
	//		}

	//		//Bind function.
	//		{
	//			Code realization;
	//			realization.Add("context.GetGlobalNamespace()");
	//			realization.NewLine();
	//			realization.Add("\t.beginClass<" + componentEcsFile->GetName() + ">(\"" + componentEcsFile->GetName() + "\")");
	//			realization.NewLine();
	//			componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
	//				if (IsBindableType(fieldInfo.GetTypeName())) {
	//					realization.Add("\t.addProperty (\"" + fieldInfo.GetName() + "\", &" + componentEcsFile->GetName() + "::" + fieldInfo.GetName() + "_)");
	//					realization.NewLine();
	//				}
	//				return true;
	//				});
	//			realization.Add("\t.endClass();");
	//			realization.NewLine();

	//			Function::CreateInfo fci{
	//				.name_ = "Bind" + componentEcsFile->GetName(),
	//				.parameters_ = {
	//					{ "::Lua::Context&", "context" }
	//				},
	//				.returnType_ = "void",
	//				.code_ = realization,
	//				.inlineModifier_ = true
	//			};

	//			auto editFunction = std::make_shared<Function>(fci);

	//			namespaceObject->Add(editFunction);
	//		}


	//		//Add function.
	//		{
	//			Code realization;
	//			if (componentEcsFile->CanBeCreatedFromImGui()) {
	//				componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
	//					realization.Add(GenerateTypeImGuiInputVariable(
	//						fieldInfo.GetTypeName(),
	//						fieldInfo.GetName())
	//						.NewLine());
	//					return true;
	//					});
	//			}
	//			realization.NewLine();
	//			realization.Add("if (ImGui::CollapsingHeader(\"Create info\")) {");
	//			{
	//				realization.NewLine();

	//				if (componentEcsFile->CanBeCreatedFromImGui()) {
	//					componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
	//						std::string variableName;
	//						if (!(fieldInfo.GetTypeName() == "std::string")) {
	//							variableName += "&"; // If string we dont need &, because array id is pointer.
	//						}
	//						variableName += fieldInfo.GetName();
	//						Code addRealization =
	//							GenerateImGuiInputTypeCode(
	//								fieldInfo.GetName(),
	//								fieldInfo.GetTypeName(),
	//								variableName);
	//						addRealization.ApplyTab();
	//						realization.Add(addRealization);
	//						realization.NewLine();
	//						return true;
	//						});
	//				}


	//				realization.Add("ImGui::Spacing();");

	//				realization.NewLine();
	//				realization.Add("\tif (ImGui::Button(\"Add component\")) {");
	//				{
	//					realization.NewLine();
	//					realization.Add("if (!world->IsComponentExist<" + componentEcsFile->GetName() + ">(entityId)) {");
	//					realization.NewLine();

	//					realization.Add("world->CreateComponent<" + componentEcsFile->GetName() + ">(entityId");
	//					if (componentEcsFile->CanBeCreatedFromImGui()) {
	//						if (componentEcsFile->AreThereFields()) {
	//							realization.Add(", ");
	//						}
	//						componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {
	//							realization.Add(fieldInfo.GetName());
	//							if (!isLast) {
	//								realization.Add(", ");
	//							}
	//							return true;
	//							});
	//					}
	//					realization.Add(");");
	//					realization.NewLine();


	//					realization.Add("}");
	//				}
	//				realization.NewLine();
	//				realization.NewLine();
	//				realization.Add("}");
	//			}
	//			realization.NewLine();
	//			realization.Add("}");
	//			Function::CreateInfo fci{
	//				.name_ = "Add" + componentEcsFile->GetName(),
	//				.parameters_ = {
	//					{ "ECS2::World*", "world" },
	//					{ "ECS2::Entity::Id", "entityId" }
	//				},
	//				.returnType_ = "void",
	//				.code_ = realization,
	//				.inlineModifier_ = true
	//			};

	//			auto editFunction = std::make_shared<Function>(fci);

	//			namespaceObject->Add(editFunction);
	//		}

	//		File::Includes includes{
	//			{	"imgui.h",
	//				"ECS2.World.hpp",
	//				"ECS2.Entity.hpp",
	//				"ECS2.Component.hpp",
	//				"Lua.Context.hpp" }
	//		};

	//		componentEcsFile->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {

	//			if (fieldInfo.GetTypeName() == "std::string") {
	//				includes.paths_.push_back("string");
	//			}
	//			if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
	//				includes.paths_.push_back("Common.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
	//				includes.paths_.push_back("chrono");
	//			}
	//			if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
	//				includes.paths_.push_back("PAL.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
	//				includes.paths_.push_back("glm/glm.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
	//				includes.paths_.push_back("queue");
	//			}
	//			if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
	//				includes.paths_.push_back("vector");
	//			}
	//			if (fieldInfo.GetTypeName().find("RAL") != std::string::npos) {
	//				includes.paths_.push_back("RAL.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
	//				includes.paths_.push_back("Geometry.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
	//				includes.paths_.push_back("UIAL.Window.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("UI::Window") != std::string::npos) {
	//				includes.paths_.push_back("UI/OksEngine.UI.Window.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("sf") != std::string::npos) {
	//				includes.paths_.push_back("SFML/Audio.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
	//				includes.paths_.push_back("glm/gtc/quaternion.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("PE") != std::string::npos) {
	//				includes.paths_.push_back("PE.PhysicsEngine.hpp");
	//			}
	//			if (fieldInfo.GetTypeName().find("AsyncResourceSubsystem") != std::string::npos) {
	//				includes.paths_.push_back("Resources/OksEngine.Resource.Subsystem.hpp");
	//			}

	//			return true;
	//			});

	//		File::CreateInfo fci{
	//			.isHpp_ = true,
	//			.includes_ = includes,
	//			.base_ = namespaceObject
	//		};
	//		auto file = std::make_shared<File>(fci);

	//		std::filesystem::path componentHppFileFullPath
	//			= componentEcsFile->GetPath().parent_path() / ("auto_OksEngine." + componentEcsFile->GetName() + ".hpp");
	//		std::string componentHppFileFullPathString = componentHppFileFullPath.string();
	//		std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
	//		return { std::filesystem::path{ componentHppFileFullPathString }, file };

	//	}

	//	CreateInfo ci_;
	//};



}