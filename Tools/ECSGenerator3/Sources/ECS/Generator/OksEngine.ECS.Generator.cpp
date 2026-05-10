#pragma once
#include <ECS\Generator\OksEngine.ECS.Generator.Utils.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <CPP/OksEngine.CPP.Tree.Utils.hpp>
#include <ECS/File/OksEngine.ECS.File.Utils.hpp>


namespace OksEngine::ECS::Generator
{


	//.ecs file to .hpp file.
	void GenerateTrees::Update(
		ECS2::Entity::Id entity0id,
		const OksEngine::CommandLineParameters* commandLineParameters0,
		const OksEngine::ECS::ProjectFilePath* projectFilePath0) {


		const auto projectFilePath = std::filesystem::path{ projectFilePath0->path_ };

		std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(projectFilePath.string());

		::Lua::Context context;
		::Lua::Script script{ std::string{ data.data(), data.size() } };
		context.LoadScript(script);

		luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");

		ASSERT_FMSG(ecsTable.isTable() || ecsTable["Modules"].isTable(), "");

		luabridge::LuaRef modulesTable = ecsTable["Modules"];

		const auto allFiles = GetComponents<ECS__FILE__FILE>();
		const Common::Size allFilesNumber = world_->GetEntitiesNumber<ECS__FILE__FILE>();

		Common::Size componentTypeId = 0;

		for (Common::Index i = 0; i < allFilesNumber; i++) {

			const auto* path = std::get<ECS::File::Path*>(allFiles) + i;

			//HPP File
			ECS2::Entity::Id hppFileEntityId = CreateEntity<CPP__FILE__FILE>();
			std::vector<ECS2::Entity::Id> hppFileNodeEntityIds;
			{
				CreateComponent<CPP::File::Tag>(hppFileEntityId);
				CreateComponent<CPP::File::Type::Hpp>(hppFileEntityId);
				//File PATH and NAME
				CreateComponent<CPP::File::Path>(hppFileEntityId, std::filesystem::path{ path->path_ }.parent_path().string());
				CreateComponent<CPP::File::Name>(hppFileEntityId, "auto_" + std::filesystem::path{ path->path_ }.stem().string());

				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_PRAGMA_ONCE());
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("ECS2.hpp"));
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("chrono"));
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("tuple"));
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("OksEngine.IComponent.hpp"));
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("Common.hpp"));
				hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE("Common.Bitset.hpp"));
			}

			//CPP File
			ECS2::Entity::Id cppFileEntityId = CreateEntity<CPP__FILE__FILE>();
			std::vector<ECS2::Entity::Id> cppFileNodeEntityIds;
			{
				CreateComponent<CPP::File::Tag>(cppFileEntityId);
				CreateComponent<CPP::File::Type::Cpp>(cppFileEntityId);

				//File PATH and NAME
				CreateComponent<CPP::File::Path>(cppFileEntityId, std::filesystem::path{ path->path_ }.parent_path().string());
				CreateComponent<CPP::File::Name>(cppFileEntityId, "auto_" + std::filesystem::path{ path->path_ }.stem().string());

				//Generate .hpp file include.
				{
					auto projectFilePathIt = projectFilePath.begin();
					std::filesystem::path hppFilePath = std::filesystem::path{ path->path_ };
					auto hppFilePathIt = hppFilePath.begin();

					while (*projectFilePathIt == *hppFilePathIt) {
						++projectFilePathIt;
						++hppFilePathIt;
					}
					std::filesystem::path hppFileRelativePath;
					ASSERT(*hppFilePathIt == "Sources");
					++hppFilePathIt;
					while (hppFilePathIt != hppFilePath.end()) {
						hppFileRelativePath /= *hppFilePathIt;
						++hppFilePathIt;
					}
					hppFileRelativePath = hppFileRelativePath.parent_path() / ("auto_" + hppFileRelativePath.stem().string() + ".hpp");

					cppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE(hppFileRelativePath.string()));
				}
			}

			//All file TABLES
			std::vector<ECS2::Entity::Id> fileStructs;
			std::vector<ECS2::Entity::Id> fileConstants;
			std::vector<ECS2::Entity::Id> fileComponents;
			std::vector<ECS2::Entity::Id> fileSystems;
			std::vector<ECS2::Entity::Id> fileEnums;
			std::vector<ECS2::Entity::Id> fileArchetypes;

			std::function<void(ECS2::Entity::Id)> getFileTables = [&](ECS2::Entity::Id entityId) {


				const ECS2::ComponentsFilter tableComponentsFilter = GetComponentsFilter(entityId);

				//Table is namespace.
				if (tableComponentsFilter.IsSet<File::Table::Namespace::Tag>()) {
					for (ECS2::Entity::Id childTableEntityId : GetComponent<File::Table::ChildTablesEntityIds>(entityId)->entityIds_) {
						getFileTables(childTableEntityId);
					}
				}
				else if (tableComponentsFilter.IsSet<File::Table::System::Tag>()) {
					fileSystems.push_back(entityId);
				}
				else if (tableComponentsFilter.IsSet<File::Table::Component::Tag>()) {
					fileComponents.push_back(entityId);
				}
				else if (tableComponentsFilter.IsSet<File::Table::Struct::Tag>()) {
					fileStructs.push_back(entityId);
				}
				else if (tableComponentsFilter.IsSet<File::Table::Constant::Tag>()) {
					fileConstants.push_back(entityId);
				}
				else if (tableComponentsFilter.IsSet<File::Table::Archetype::Tag>()) {
					fileArchetypes.push_back(entityId);
				}
				else if (tableComponentsFilter.IsSet<File::Table::Enum::Tag>()) {
					fileEnums.push_back(entityId);
				}
				else {
					ASSERT_FAIL();
				}
				};

			const auto* fileTables = std::get<ECS::File::Table::EntityIds*>(allFiles) + i;
			for (ECS2::Entity::Id tableEntityId : fileTables->entityIds_) {
				getFileTables(tableEntityId);
			}

			//Generate constants code tree.
			for (ECS2::Entity::Id constantEntityId : fileConstants) {
				const ECS2::ComponentsFilter constantCF = GetComponentsFilter(constantEntityId);
				ASSERT(constantCF.IsSet<File::Table::Constant::Tag>());

				const ECS2::Entity::Id cppConstantEntityId = CreateEntity();
				const auto parentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(constantEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(parentTables, std::vector<ECS2::Entity::Id>{cppConstantEntityId});

				CPP__TREE__DECL__CREATE_VARIABLE_COMPONENTS(
					cppConstantEntityId,
					CPP__TREE__TYPE__CREATE_NAMED_TYPE(GetComponent<File::Table::Constant::TypeName>(constantEntityId)->type_),
					GetComponent<File::Table::Name>(constantEntityId)->name_,
					CPP__TREE__EXPR__CREATE_LITERAL_EXPR(GetComponent<File::Table::Constant::Value>(constantEntityId)->value_),
					lastNamespaceEntityId
				);

				if (firstNamespaceEntityId.IsValid()) {
					hppFileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					hppFileNodeEntityIds.push_back(cppConstantEntityId);
				}
			}

			//Generate structs code tree.
			for (ECS2::Entity::Id structEntityId : fileStructs) {

				const ECS2::ComponentsFilter structCF = GetComponentsFilter(structEntityId);
				ASSERT(structCF.IsSet<File::Table::Struct::Tag>());

				const ECS2::Entity::Id cppStructEntityId = CreateEntity();
				const auto ecsParentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(structEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(ecsParentTables, std::vector<ECS2::Entity::Id>{cppStructEntityId});

				{
					CreateComponent<CPP::Tree::Node::Tag>(cppStructEntityId);
					CreateComponent<CPP::Tree::Decl::Tag>(cppStructEntityId);
					CreateComponent<CPP::Tree::Decl::Class_>(cppStructEntityId);
					CreateComponent<CPP::Tree::Node::Name>(cppStructEntityId, GetComponent<File::Table::Name>(structEntityId)->name_);

					std::vector<ECS2::Entity::Id> childEntityIds;
					std::vector<ECS2::Entity::Id> fieldEntityIds;
					if (IsComponentExist<ECS::File::Table::Struct::Field::EntityIds>(structEntityId)) {
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Struct::Field::EntityIds>(structEntityId)->entityIds_) {
							ASSERT(IsComponentExist<ECS::File::Table::Struct::Field::Tag>(fieldEntityId));

							ECS2::Entity::Id variableEntityId = CPP__TREE__DECL__CREATE_VARIABLE(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE(GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_),
								GetComponent<File::Table::Struct::Field::Name>(fieldEntityId)->name_ + "_",
								(IsComponentExist<File::Table::Struct::Field::DefaultValue>(fieldEntityId))
								? (CPP__TREE__EXPR__CREATE_LITERAL_EXPR(GetComponent<File::Table::Struct::Field::DefaultValue>(fieldEntityId)->value_))
								: (ECS2::Entity::Id::invalid_),
								cppStructEntityId
							);
							CreateComponent<CPP::Tree::Access::Public_>(variableEntityId);

							fieldEntityIds.push_back(variableEntityId);
							childEntityIds.push_back(variableEntityId);
						}
					}
					//Default constructor.
					{
						ECS2::Entity::Id constructorEntityId = CPP__TREE__DECL__CREATE_EMPTY_CONSTRUCTOR(
							GetComponent<File::Table::Name>(structEntityId)->name_,
							cppStructEntityId);
						CreateComponent<CPP::Tree::Decl::Constructor>(constructorEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(constructorEntityId);

						childEntityIds.push_back(constructorEntityId);
					}
					//Constructor with parameters.
					if (!fieldEntityIds.empty()) {
						ECS2::Entity::Id constructorWithParametersEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Decl::Function>(constructorWithParametersEntityId, ECS2::Entity::Id::invalid_);//No return value for constructor.
						CreateComponent<CPP::Tree::Decl::Constructor>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Node::Name>(constructorWithParametersEntityId, GetComponent<File::Table::Name>(structEntityId)->name_);
						CreateComponent<CPP::Tree::Access::Public_>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(constructorWithParametersEntityId, cppStructEntityId);

						std::vector<ECS2::Entity::Id> parameterEntityIds;
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Struct::Field::EntityIds>(structEntityId)->entityIds_) {
							GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_;

							ECS2::Entity::Id parameterEntityId = CreateEntity();
							CreateComponent<CPP::Tree::Node::Tag>(parameterEntityId);
							CreateComponent<CPP::Tree::Decl::Tag>(parameterEntityId);

							//Type entity id.
							ECS2::Entity::Id typeEntityId = CPP__TREE__TYPE__CREATE_NAMED_TYPE(GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_);

							CreateComponent<CPP::Tree::Decl::Parameter>(parameterEntityId,
								GetComponent<ECS::File::Table::Struct::Field::Name>(fieldEntityId)->name_,
								typeEntityId,
								ECS2::Entity::Id::invalid_ // There is no initializer.
							);
							parameterEntityIds.push_back(parameterEntityId);
						}

						//Initialize list.
						{
							std::vector<ECS2::Entity::Id> initializerEntityIds;
							for (Common::Index i = 0; i < fieldEntityIds.size(); i++) {
								ECS2::Entity::Id fieldEntityId = fieldEntityIds[i];

								ECS2::Entity::Id initializeExpr = CreateEntity();
								CreateComponent<CPP::Tree::Expr::Tag>(initializeExpr);
								//CreateComponent<CPP::Tree::Expr::Identifier>(initializeExpr, GetComponent<CPP::Tree::Decl::Parameter>(parameterEntityIds[i])->name_);
								CreateComponent<CPP::Tree::Node::ResolvedRef>(initializeExpr, parameterEntityIds[i]);

								ECS2::Entity::Id initializer = CreateEntity();
								CreateComponent<CPP::Tree::ConstructorInitializer>(initializer,
									fieldEntityId, // Member for initialization.
									initializeExpr // Initialize expr.
								);
								initializerEntityIds.push_back(initializer);
							}
							CreateComponent<CPP::Tree::ConstructorInitList>(constructorWithParametersEntityId, initializerEntityIds);
						}

						parameterEntityIds.push_back(CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT());
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(constructorWithParametersEntityId, parameterEntityIds); // No body content.
						childEntityIds.push_back(constructorWithParametersEntityId);

					}
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppStructEntityId, childEntityIds);
				}
				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppStructEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					hppFileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					hppFileNodeEntityIds.push_back(cppStructEntityId);
				}
			}

			//Generate components code tree.
			for (ECS2::Entity::Id componentEntityId : fileComponents) {

				const ECS2::ComponentsFilter componentCF = GetComponentsFilter(componentEntityId);
				ASSERT(componentCF.IsSet<File::Table::Component::Tag>());

				const ECS2::Entity::Id cppComponentEntityId = CreateEntity();
				const auto ecsParentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(componentEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(ecsParentTables, std::vector<ECS2::Entity::Id>{cppComponentEntityId});

				{
					CreateComponent<CPP::Tree::Node::Tag>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Decl::Tag>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Decl::Class_>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Node::Name>(cppComponentEntityId, GetComponent<File::Table::Name>(componentEntityId)->name_);

					std::vector<ECS2::Entity::Id> childEntityIds;
					std::vector<ECS2::Entity::Id> cppFieldEntityIds;
					std::vector<ECS2::Entity::Id> ecsFieldEntityIds;

					if (IsComponentExist<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)) {
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)->entityIds_) {
							ASSERT(IsComponentExist<ECS::File::Table::Component::Field::Tag>(fieldEntityId));
							ecsFieldEntityIds.push_back(fieldEntityId);
							ECS2::Entity::Id variableEntityId = CPP__TREE__DECL__CREATE_VARIABLE(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE(GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_),				/*Type*/
								GetComponent<File::Table::Component::Field::Name>(fieldEntityId)->name_ + "_",												/*Name*/
								(IsComponentExist<File::Table::Component::Field::DefaultValue>(fieldEntityId))												/*Initialize if need*/
								? (CPP__TREE__EXPR__CREATE_LITERAL_EXPR(GetComponent<File::Table::Component::Field::DefaultValue>(fieldEntityId)->value_))
								: (ECS2::Entity::Id::invalid_),
								cppComponentEntityId																										/*Parent*/
							);

							CreateComponent<CPP::Tree::Access::Public_>(variableEntityId);
							cppFieldEntityIds.push_back(variableEntityId);
							childEntityIds.push_back(variableEntityId);
						}
					}
					//Default constructor.
					{
						ECS2::Entity::Id constructorEntityId = CPP__TREE__DECL__CREATE_EMPTY_CONSTRUCTOR(
							GetComponent<File::Table::Name>(componentEntityId)->name_,
							cppComponentEntityId);
						CreateComponent<CPP::Tree::Decl::Constructor>(constructorEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(constructorEntityId);
						childEntityIds.push_back(constructorEntityId);
					}
					//Constructor with parameters.
					if (!cppFieldEntityIds.empty()) {

						//Parameters.
						std::vector<ECS2::Entity::Id> parameterEntityIds;
						if (IsComponentExist<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)) {
							for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)->entityIds_) {
								parameterEntityIds.push_back(
									CPP__TREE__DECL__CREATE_PARAMETER(
										CPP__TREE__TYPE__CREATE_NAMED_TYPE(GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_),
										GetComponent<ECS::File::Table::Component::Field::Name>(fieldEntityId)->name_
									)
								);
							}
						}
						//Add empty body.
						parameterEntityIds.push_back(CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT());

						ECS2::Entity::Id constructorWithParametersEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
							ECS2::Entity::Id::invalid_,
							GetComponent<File::Table::Name>(componentEntityId)->name_,
							cppComponentEntityId,
							parameterEntityIds
						);


						//Initialize list.
						{
							std::vector<ECS2::Entity::Id> initializerEntityIds;
							for (Common::Index i = 0; i < ecsFieldEntityIds.size(); i++) {
								ECS2::Entity::Id fieldEntityId = ecsFieldEntityIds[i];
								//fieldEntityIds[i]{ parameterEntityIds[i] }
								ECS2::Entity::Id initializer = CPP__TREE__CREATE_CONSTRUCTOR_INITIALIZER(
									cppFieldEntityIds[i],
									CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR(GetComponent<ECS::File::Table::Component::Field::Name>(fieldEntityId)->name_)
								);
								initializerEntityIds.push_back(initializer);
							}
							if (!initializerEntityIds.empty()) {
								CreateComponent<CPP::Tree::ConstructorInitList>(constructorWithParametersEntityId, initializerEntityIds);
							}
						}

						CreateComponent<CPP::Tree::Decl::Constructor>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(constructorWithParametersEntityId);

						childEntityIds.push_back(constructorWithParametersEntityId);
					}

					//GetName static Function
					//static const char* GetName()
					//{
					//	return "Config::Tag";
					//};
					{
						const std::string componentFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
						ECS2::Entity::Id getNameFunctionEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
							CPP__TREE__TYPE__CREATE_POINTER_TYPE(CPP__TREE__TYPE__CREATE_CONST_CHAR_TYPE()),
							"GetName",
							cppComponentEntityId,
							std::vector<ECS2::Entity::Id>{
							CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(
								std::vector<ECS2::Entity::Id>{
								CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
									CPP__TREE__EXPR__CREATE_LITERAL_EXPR("\"" + componentFullName + "\"")
								)
							}
							)
						}
						);
						CreateComponent<CPP::Tree::Access::Public_>(getNameFunctionEntityId);
						CreateComponent<CPP::Tree::StorageClass::Static_>(getNameFunctionEntityId);
						childEntityIds.push_back(getNameFunctionEntityId);
					}
					//GetTypeId static Function
					//static inline ECS2::ComponentTypeId GetTypeId()
					//{
					//	return 211;
					//};
					{
						ECS2::Entity::Id getTypeIdFunctionEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
							CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::ComponentTypeId"),
							"GetTypeId",
							cppComponentEntityId,
							std::vector<ECS2::Entity::Id>{
							CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(
								std::vector<ECS2::Entity::Id>{
								CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
									CPP__TREE__EXPR__CREATE_LITERAL_EXPR(std::to_string(++componentTypeId))
								)
							}
							)
						}
						);
						CreateComponent<CPP::Tree::Access::Public_>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::StorageClass::Static_>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::Inline_>(getTypeIdFunctionEntityId);
						childEntityIds.push_back(getTypeIdFunctionEntityId);
					}

					CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppComponentEntityId, childEntityIds);
				}
				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppComponentEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					hppFileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					hppFileNodeEntityIds.push_back(cppComponentEntityId);
				}
			}

			//Generate Enums code tree.
			for (ECS2::Entity::Id enumEntityId : fileEnums) {
				const ECS2::ComponentsFilter enumCF = GetComponentsFilter(enumEntityId);
				ASSERT(enumCF.IsSet<File::Table::Enum::Tag>());

				const ECS2::Entity::Id cppEnumEntityId = CreateEntity();
				const auto ecsParentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(enumEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(ecsParentTables, std::vector<ECS2::Entity::Id>{cppEnumEntityId});

				CreateComponent<CPP::Tree::Node::Tag>(cppEnumEntityId);
				CreateComponent<CPP::Tree::Decl::Tag>(cppEnumEntityId);
				CreateComponent<CPP::Tree::Decl::Enum_>(cppEnumEntityId);
				CreateComponent<CPP::Tree::Node::Name>(cppEnumEntityId, GetComponent<File::Table::Name>(enumEntityId)->name_);

				std::vector<ECS2::Entity::Id> enumeratorEntityIds;
				for (const std::string value : GetComponent<ECS::File::Table::Enum::Values>(enumEntityId)->values_) {
					ECS2::Entity::Id enumeratorEntityId = CreateEntity();
					CreateComponent<CPP::Tree::Node::Tag>(enumeratorEntityId);
					CreateComponent<CPP::Tree::Decl::Tag>(enumeratorEntityId);
					CreateComponent<CPP::Tree::Decl::Enumerator>(enumeratorEntityId, value, ECS2::Entity::Id::invalid_); // Without 
					CreateComponent<CPP::Tree::Node::Name>(enumeratorEntityId, value);
					CreateComponent<CPP::Tree::Node::ParentEntityId>(enumeratorEntityId, cppEnumEntityId);
					enumeratorEntityIds.push_back(enumeratorEntityId);
				}
				CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppEnumEntityId, enumeratorEntityIds);

				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppEnumEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					hppFileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					hppFileNodeEntityIds.push_back(cppEnumEntityId);
				}
			}

			//Generate Systems code tree.
			BEGIN_PROFILE("Generate systems code tree for file %s", path->path_.c_str());
			for (ECS2::Entity::Id systemEntityId : fileSystems) {
				const ECS2::ComponentsFilter systemCF = GetComponentsFilter(systemEntityId);
				ASSERT(systemCF.IsSet<File::Table::System::Tag>());
				const std::string systemName = GetComponent<File::Table::Name>(systemEntityId)->name_;

				//HPP file code.
				{
					const ECS2::Entity::Id runSystemFunctionPrototypeEntityId = CreateEntity();
					const ECS2::Entity::Id cppSystemClassEntityId = CreateEntity();
					std::vector<ECS2::Entity::Id> namespaceChilds{ cppSystemClassEntityId, runSystemFunctionPrototypeEntityId };

					const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(
						ECS__FILE__TABLE__GET_TABLE_PARENTS(systemEntityId),
						namespaceChilds);

					//System class 
					{
						std::vector<ECS2::Entity::Id> systemClassChildEntityIds;

						//Default constructor.
						{
							ECS2::Entity::Id constructorEntityId = CPP__TREE__DECL__CREATE_EMPTY_CONSTRUCTOR(
								GetComponent<File::Table::Name>(systemEntityId)->name_,
								cppSystemClassEntityId);
							CreateComponent<CPP::Tree::Decl::Constructor>(constructorEntityId);
							CreateComponent<CPP::Tree::Access::Public_>(constructorEntityId);
							systemClassChildEntityIds.push_back(constructorEntityId);
						}
						//std::shared_ptr<ECS2::World> world_{ nullptr } member
						ECS2::Entity::Id worldMemberEntityId = CPP__TREE__DECL__CREATE_VARIABLE(
							CPP__TREE__TYPE__CREATE_TEMPLATE_SPECIALIZATION_TYPE(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE("std::shared_ptr"),
								std::vector{ CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::World") }),
							"world_",
							CPP__TREE__EXPR__CREATE_LITERAL_EXPR("nullptr"),
							cppSystemClassEntityId
						);

						systemClassChildEntityIds.push_back(worldMemberEntityId);

						//Constructor with parameter.
						//Create(std::shared_ptr<ECS2::World> world)
						//	:

						//	world_{ world }
						//{
						//}
						{
							ECS2::Entity::Id worldParameterEntityId = CPP__TREE__DECL__CREATE_PARAMETER(
								CPP__TREE__TYPE__CREATE_TEMPLATE_SPECIALIZATION_TYPE(
									CPP__TREE__TYPE__CREATE_NAMED_TYPE("std::shared_ptr"),
									std::vector{ CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::World") }),
								"world");
							std::vector<ECS2::Entity::Id> functionChilds{ worldParameterEntityId, CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT() };
							ECS2::Entity::Id constructorWithParametersEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
								ECS2::Entity::Id::invalid_,
								GetComponent<File::Table::Name>(systemEntityId)->name_,
								cppSystemClassEntityId,
								functionChilds
							);
							CreateComponent<CPP::Tree::Access::Public_>(constructorWithParametersEntityId);
							//Initialize list.
							CreateComponent<CPP::Tree::Decl::Constructor>(constructorWithParametersEntityId);
							CreateComponent<CPP::Tree::ConstructorInitList>(constructorWithParametersEntityId,
								std::vector<ECS2::Entity::Id>{ CPP__TREE__CREATE_CONSTRUCTOR_INITIALIZER(worldMemberEntityId, CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world"))});

							systemClassChildEntityIds.push_back(constructorWithParametersEntityId);
						}

						CPP__TREE__DECL__CREATE_CLASS_COMPONENTS(
							cppSystemClassEntityId,
							GetComponent<File::Table::Name>(systemEntityId)->name_,
							lastNamespaceEntityId,
							systemClassChildEntityIds
						);

						if (firstNamespaceEntityId.IsInvalid()) {
							hppFileNodeEntityIds.push_back(cppSystemClassEntityId);
						}
					}
					//Create run system prototype for HPP 
					{

						//Parameter: std::shared_ptr<ECS2::World> world
						ECS2::Entity::Id worldParameterEntityId = CPP__TREE__DECL__CREATE_PARAMETER(
							CPP__TREE__TYPE__CREATE_TEMPLATE_SPECIALIZATION_TYPE(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE("std::shared_ptr"),
								std::vector{ CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::World") }),
							"world");

						CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
							runSystemFunctionPrototypeEntityId,
							CPP__TREE__TYPE__CREATE_VOID_TYPE(),
							GetComponent<File::Table::Name>(systemEntityId)->name_ + "System",
							lastNamespaceEntityId,
							std::vector{ worldParameterEntityId }
						);

						if (firstNamespaceEntityId.IsInvalid()) {
							hppFileNodeEntityIds.push_back(runSystemFunctionPrototypeEntityId);
						}

					}

					if (firstNamespaceEntityId.IsValid()) {
						hppFileNodeEntityIds.push_back(firstNamespaceEntityId);
					}

				}

				//Create run system realization for CPP 
				{
					ECS2::Entity::Id runSystemFunctionRealizationEntityId = CreateEntity();
					const auto ecsParentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(systemEntityId);
					std::vector<ECS2::Entity::Id> namespaceChilds{ runSystemFunctionRealizationEntityId };
					const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(ecsParentTables, namespaceChilds);


					std::vector<ECS2::Entity::Id> functionChilds;
					//Parameter: std::shared_ptr<ECS2::World> world
					ECS2::Entity::Id worldParameterEntityId = CPP__TREE__DECL__CREATE_PARAMETER(
						CPP__TREE__TYPE__CREATE_TEMPLATE_SPECIALIZATION_TYPE(
							CPP__TREE__TYPE__CREATE_NAMED_TYPE("std::shared_ptr"),
							std::vector{ CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::World") }),
						"world");
					functionChilds.push_back(worldParameterEntityId);

					auto getTableByName = [this]<typename TableTag>(ECS2::Entity::Id usageTableEntityId, const std::string & findingFullTableName) {
						ASSERT(ECS::File::Table::IsValidName(findingFullTableName));
						const std::string findingComponentName = ECS::File::Table::GetName(findingFullTableName);
						std::vector<std::string> systemNamespaceNames = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(usageTableEntityId);
						systemNamespaceNames.insert(systemNamespaceNames.begin(), "Root");

						std::vector<std::string> findingTableSplitedFullName = ECS::File::Table::SplitName(findingFullTableName);
						if (findingFullTableName.starts_with("::")) {
							findingTableSplitedFullName.insert(findingTableSplitedFullName.begin(), "Root");
						}
						const std::string findingTableName = findingTableSplitedFullName.back();

						// remove last element with name to get only namespaces
						std::vector<std::string> findingTableSplitedNamespaces = findingTableSplitedFullName;
						findingTableSplitedNamespaces.pop_back();


						std::vector<std::pair<Common::Size, ECS2::Entity::Id>> applicants;

						world_->ForEachEntity<ECS::File::Table::Tag>([&](ECS2::Entity::Id tableEntityId) {

							const ECS2::ComponentsFilter tableCF = GetComponentsFilter(tableEntityId);
							if (!tableCF.IsSet<TableTag>()) {
								return;
							}

							const std::string tableName = GetComponent<ECS::File::Table::Name>(tableEntityId)->name_;
							if (tableName != findingTableName) {
								//Skip table with another name.
								return;
							}

							std::vector<std::string> tableSplitedNamespaces = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);
							tableSplitedNamespaces.insert(tableSplitedNamespaces.begin(), "Root");

							bool isFindingAndCurrentTablesHasCommonNamespace = [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
								if (second.empty()) {
									return true;
								}
								for (auto rIt = first.rbegin(); rIt != first.rend(); ++rIt) {
									if (*rIt == *second.begin()) {
										const Common::UInt64 offset = std::distance(rIt, first.rend()) - 1;
										if (first.size() - offset != second.size()) {
											return false;
										}
										for (Common::Index j = offset; j < first.size(); j++) {
											if (first[j] != second[j - offset]) {
												return false;
											}
										}
										return true;

									}
								}
								return false;

								}(tableSplitedNamespaces, findingTableSplitedNamespaces);

							if (!isFindingAndCurrentTablesHasCommonNamespace) {
								return;
							}

							for (auto rIt = systemNamespaceNames.rbegin(); rIt != systemNamespaceNames.rend(); ++rIt) {
								if (*rIt == tableSplitedNamespaces.front()) {
									applicants.push_back({ std::abs(std::distance(rIt, systemNamespaceNames.rbegin())), tableEntityId });
								}
							}


							});

						if (applicants.empty()) {
							ASSERT_FAIL();
						}

						std::sort(applicants.begin(), applicants.end(),
							[](std::pair<Common::Size, ECS2::Entity::Id>& first, std::pair<Common::Size, ECS2::Entity::Id>& second) {
								return	first.first < second.first;
							});

						return applicants.front().second;


					};

					//Body.
					ECS2::Entity::Id bodyCompoundStmtEntityId = CreateEntity();
					{
						std::vector<ECS2::Entity::Id> bodyChilds;
						{//System object variable.
							ECS2::Entity::Id systemObjectVariable = CPP__TREE__DECL__CREATE_VARIABLE(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE(systemName),
								std::string{ (char)std::tolower(systemName[0]) } + systemName.substr(1),
								CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world"),
								bodyCompoundStmtEntityId
							);
							bodyChilds.push_back(systemObjectVariable);
						}

						//Processing entities
						if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
							ECS2::Entity::Id updateMethodEntityId = GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
							if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {
								const auto processEntityIds = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;

								std::vector<ECS2::Entity::Id> updateCallArgumentsIdentifierExprEntityIds;

								std::function<ECS2::Entity::Id(ECS2::Entity::Id, Common::Index)> createForEachCall = [&](ECS2::Entity::Id processEntityId, Common::Index entityIndex) {

									if (entityIndex == processEntityIds.size()) {
										return CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT();
									}
									const bool isLastEntity = entityIndex == processEntityIds.size() - 1;
									auto processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processEntityId)->componentInfos_;
									auto excludeComponents = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ExcludeComponents>(processEntityId)->components_;
									//Exclude ComponentsFilter
									{
										ECS2::Entity::Id excludeVariable = CPP__TREE__DECL__CREATE_VARIABLE(
											CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::ComponentsFilter"),
											"excludesForEntity" + std::to_string(entityIndex),
											ECS2::Entity::Id::invalid_,
											bodyCompoundStmtEntityId
										);
										ECS2::Entity::Id memberAccessExprEntityId = CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
											CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("excludesForEntity" + std::to_string(entityIndex)),
											"SetBits",
											false);

										std::vector<ECS2::Entity::Id> templateArgEntityIds;
										for (const auto& excludeComponent : excludeComponents) {
											ECS2::Entity::Id excludeComponentEntityId = getTableByName.operator() < ECS::File::Table::Component::Tag > (systemEntityId, excludeComponent);
											const std::string componentTypeFullName = ECS__FILE__TABLE__GET_FULL_NAME(excludeComponentEntityId, "::", false);
											ECS2::Entity::Id componentTypeEntityId = CPP__TREE__TYPE__CREATE_NAMED_TYPE(componentTypeFullName);
											templateArgEntityIds.push_back(componentTypeEntityId);
										}
									
										bodyChilds.push_back(excludeVariable);
										bodyChilds.push_back(CPP__TREE__STMT__CREATE_EXPRESSION_STATEMENT(
											CPP__TREE__EXPR__CREATE_CALL_EXPR(memberAccessExprEntityId, templateArgEntityIds, std::vector<ECS2::Entity::Id>{})
										));
									}

									ECS2::Entity::Id lambdaEntityId = ECS2::Entity::Id::invalid_;
									{
										std::vector<ECS2::Entity::Id> lambdaParameters;
										//ECS2::Entity::Id entity0Id
										lambdaParameters.push_back(CPP__TREE__DECL__CREATE_PARAMETER(
											CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"),
											"entity" + std::to_string(entityIndex) + "Id")
										);
										updateCallArgumentsIdentifierExprEntityIds.push_back(CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("entity" + std::to_string(entityIndex) + "Id"));
										for (const auto& componentInfo : processComponentInfos) {
											ECS2::Entity::Id componentEntityId = getTableByName.operator() < ECS::File::Table::Component::Tag > (systemEntityId, componentInfo.name_);
											const std::string componentTypeFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
											std::string componentParameterFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "__", true);
											componentParameterFullName = std::string{ (char)std::tolower(componentParameterFullName[0]) } + componentParameterFullName.substr(1);
											updateCallArgumentsIdentifierExprEntityIds.push_back(CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR(componentParameterFullName));
											ECS2::Entity::Id componentTypeEntityId = CPP__TREE__TYPE__CREATE_NAMED_TYPE(componentTypeFullName);
											if (componentInfo.readonly_) {
												CreateComponent<CPP::Tree::Type::ConstType>(componentTypeEntityId);
											}
											lambdaParameters.push_back(CPP__TREE__DECL__CREATE_PARAMETER(
												CPP__TREE__TYPE__CREATE_POINTER_TYPE(componentTypeEntityId),
												componentParameterFullName
											));
										}

										std::vector<ECS2::Entity::Id> childForEachCallStmt;
										if (!isLastEntity) {
											childForEachCallStmt.push_back(createForEachCall(processEntityIds[entityIndex + 1], entityIndex + 1));
										}
										else {
											ECS2::Entity::Id systemObjectMemberAccessExprEntityId = CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
												CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR(std::string{ (char)std::tolower(systemName[0]) } + systemName.substr(1)),
												"Update",
												false);
											childForEachCallStmt.push_back(CPP__TREE__STMT__CREATE_EXPRESSION_STATEMENT(
												CPP__TREE__EXPR__CREATE_CALL_EXPR(systemObjectMemberAccessExprEntityId, std::vector<ECS2::Entity::Id>{}, updateCallArgumentsIdentifierExprEntityIds)
											));
										}
										lambdaEntityId = CPP__TREE__EXPR__CREATE_LAMBDA(
											CPP::Tree::Expr::CaptureType::ByReference,
											std::vector<CPP::Tree::Expr::LambdaCapture>{},
											lambdaParameters,
											CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(childForEachCallStmt));
									}

									ECS2::Entity::Id memberAccessExprEntityId = CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world"),
										"ForEachEntity",
										true);

									std::vector<ECS2::Entity::Id> templateArgEntityIds;
									for (const auto& componentInfo : processComponentInfos) {
										ECS2::Entity::Id componentEntityId = getTableByName.operator() < ECS::File::Table::Component::Tag > (systemEntityId, componentInfo.name_);
										const std::string componentTypeFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
										ECS2::Entity::Id componentTypeEntityId = CPP__TREE__TYPE__CREATE_NAMED_TYPE(componentTypeFullName);
										templateArgEntityIds.push_back(componentTypeEntityId);
									}
									std::vector<ECS2::Entity::Id> argEntityIds;
									argEntityIds.push_back(CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("excludesForEntity" + std::to_string(entityIndex)));
									argEntityIds.push_back(lambdaEntityId);

									return CPP__TREE__STMT__CREATE_EXPRESSION_STATEMENT(
										CPP__TREE__EXPR__CREATE_CALL_EXPR(memberAccessExprEntityId, templateArgEntityIds, argEntityIds)
									);


									};

								ECS2::Entity::Id forEachExprStmtEntityId = createForEachCall(processEntityIds[0], 0);
								bodyChilds.push_back(forEachExprStmtEntityId);
							}
						}

						bodyCompoundStmtEntityId = CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT_COMPONENTS(
							bodyCompoundStmtEntityId,
							bodyChilds);

						functionChilds.push_back(bodyCompoundStmtEntityId);
					}


					CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
						runSystemFunctionRealizationEntityId,
						CPP__TREE__TYPE__CREATE_VOID_TYPE(),
						GetComponent<File::Table::Name>(systemEntityId)->name_ + "System",
						lastNamespaceEntityId,
						functionChilds
					);


					if (firstNamespaceEntityId.IsInvalid()) {
						cppFileNodeEntityIds.push_back(runSystemFunctionRealizationEntityId);
					}

					if (firstNamespaceEntityId.IsValid()) {
						cppFileNodeEntityIds.push_back(firstNamespaceEntityId);
					}

					////Body.
					//ECS2::Entity::Id bodyCompoundStmtEntityId = CreateEntity();
					//{
					//	CreateComponent<CPP::Tree::Node::Tag>(bodyCompoundStmtEntityId);
					//	CreateComponent<CPP::Tree::Stmt::Tag>(bodyCompoundStmtEntityId);
					//	CreateComponent<CPP::Tree::Stmt::CompoundStmt>(bodyCompoundStmtEntityId);
					//	std::vector<ECS2::Entity::Id> bodyStmts;

					//	const auto* updateMethodEntityId = GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId);
					//	if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId->id_)) {

					//		ECS2::Entity::Id systemVariableCreateStmt = CreateEntity();
					//		{
					//			CreateComponent<CPP::Tree::Node::Tag>(systemVariableCreateStmt);
					//			CreateComponent<CPP::Tree::Stmt::Tag>(systemVariableCreateStmt);
					//			CreateComponent<CPP::Tree::Stmt::CompoundStmt>(systemVariableCreateStmt);
					//		}

					//		auto* processEntityIds = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId->id_);
					//		const std::vector<ECS2::Entity::Id> processEntities = processEntityIds->entityIds_;
					//		if (!processEntities.empty()) {


					//			Common::Index i = 0;
					//			std::function<void(ECS2::Entity::Id)> processEntity = [this, &i, &processEntities, &processEntity](ECS2::Entity::Id processEntityId) {

					//				++i;
					//				if (i < processEntities.size() - 1) {
					//					processEntity(processEntities[i]);
					//				}

					//				};

					//			processEntity(processEntities[i]);

					//			for (ECS2::Entity::Id processEntityId : processEntityIds->entityIds_) {
					//				const auto* processesComponents = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processEntityId);
					//				const std::string componentName = processesComponents->componentInfos_[0].name_;

					//				ECS2::Entity::Id componentTableEntityId = [this]<typename TableTag>(ECS2::Entity::Id usageTableEntityId, const std::string & findingFullTableName) {
					//					ASSERT(ECS::File::Table::IsValidName(findingFullTableName));
					//					const std::string findingComponentName = ECS::File::Table::GetName(findingFullTableName);
					//					std::vector<std::string> systemNamespaceNames = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(usageTableEntityId);
					//					systemNamespaceNames.insert(systemNamespaceNames.begin(), "Root");

					//					std::vector<std::string> findingTableSplitedFullName = ECS::File::Table::SplitName(findingFullTableName);
					//					if (findingFullTableName.starts_with("::")) {
					//						findingTableSplitedFullName.insert(findingTableSplitedFullName.begin(), "Root");
					//					}
					//					const std::string findingTableName = findingTableSplitedFullName.back();

					//					// remove last element with name to get only namespaces
					//					std::vector<std::string> findingTableSplitedNamespaces = findingTableSplitedFullName;
					//					findingTableSplitedNamespaces.pop_back();


					//					std::vector<std::pair<Common::Size, ECS2::Entity::Id>> applicants;

					//					world_->ForEachEntity<ECS::File::Table::Tag>([&](ECS2::Entity::Id tableEntityId) {

					//						const ECS2::ComponentsFilter tableCF = GetComponentsFilter(tableEntityId);
					//						if (!tableCF.IsSet<TableTag>()) {
					//							return;
					//						}

					//						const std::string tableName = GetComponent<ECS::File::Table::Name>(tableEntityId)->name_;
					//						if (tableName != findingTableName) {
					//							//Skip table with another name.
					//							return;
					//						}

					//						std::vector<std::string> tableSplitedNamespaces = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);
					//						tableSplitedNamespaces.insert(tableSplitedNamespaces.begin(), "Root");

					//						bool isFindingAndCurrentTablesHasCommonNamespace = [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
					//							if (second.empty()) {
					//								return true;
					//							}
					//							for (auto rIt = first.rbegin(); rIt != first.rend(); ++rIt) {
					//								if (*rIt == *second.begin()) {
					//									const Common::UInt64 offset = std::distance(rIt, first.rend()) - 1;
					//									if (first.size() - offset != second.size()) {
					//										return false;
					//									}
					//									for (Common::Index j = offset; j < first.size(); j++) {
					//										if (first[j] != second[j - offset]) {
					//											return false;
					//										}
					//									}
					//									return true;

					//								}
					//							}
					//							return false;

					//							}(tableSplitedNamespaces, findingTableSplitedNamespaces);

					//						if (!isFindingAndCurrentTablesHasCommonNamespace) {
					//							return;
					//						}

					//						for (auto rIt = systemNamespaceNames.rbegin(); rIt != systemNamespaceNames.rend(); ++rIt) {
					//							if (*rIt == tableSplitedNamespaces.front()) {
					//								applicants.push_back({ std::abs(std::distance(rIt, systemNamespaceNames.rbegin())), tableEntityId });
					//							}
					//						}


					//						});

					//					if (applicants.empty()) {
					//						ASSERT_FAIL();
					//					}

					//					std::sort(applicants.begin(), applicants.end(),
					//						[](std::pair<Common::Size, ECS2::Entity::Id>& first, std::pair<Common::Size, ECS2::Entity::Id>& second) {
					//							return	first.first < second.first;
					//						});

					//					return applicants.front().second;


					//				}.operator() < ECS::File::Table::Component::Tag > (systemEntityId, componentName);


					//			}
					//		}
					//	}





					//	CreateComponent<CPP::Tree::Node::ChildEntityIds>(bodyCompoundStmtEntityId, bodyStmts);
					//}

					//CreateComponent<CPP::Tree::Node::ChildEntityIds>(runSystemFunctionRealizationEntityId,
					//	std::vector{
					//		worldParameterEntityId,
					//		bodyCompoundStmtEntityId
					//	});


				}

			}
			END_PROFILE();
			CreateComponent<CPP::Tree::Node::ChildEntityIds>(hppFileEntityId, hppFileNodeEntityIds);
			CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppFileEntityId, cppFileNodeEntityIds);
		}


		const auto allComponents = GetComponents<ECS__FILE__TABLE__COMPONENT__COMPONENT>();
		const Common::Size allComponentsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__COMPONENT__COMPONENT>();

		const auto allSystems = GetComponents<ECS__FILE__TABLE__SYSTEM__SYSTEM>();
		const Common::Size allSystemsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__SYSTEM__SYSTEM>();

	}


}; // namespace OksEngine::ECS::Generator