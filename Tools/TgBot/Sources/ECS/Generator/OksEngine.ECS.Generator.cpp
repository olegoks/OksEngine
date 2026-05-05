#pragma once
#include <ECS\Generator\auto_OksEngine.ECS.Generator.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <CPP/OksEngine.CPP.Tree.Utils.hpp>



namespace OksEngine::ECS::Generator
{
#define GET_TABLE_NAMESPACE(entityId, delimiterString, isNamespaceUppercase)\
		[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
			ECS2::ComponentsFilter beginTableCF = GetComponentsFilter(tableEntityId);\
			std::vector<std::string> namespaceStrings;\
			std::function<void(ECS2::Entity::Id)> processParent = \
				[this,\
				&namespaceStrings,\
				&processParent](ECS2::Entity::Id parentEntityId) {\
				ECS2::ComponentsFilter parentTableCF = GetComponentsFilter(parentEntityId);\
				ASSERT(IsComponentExist<File::Table::Tag>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::Namespace::Tag>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::ChildTablesEntityIds>(parentEntityId));\
				if (IsComponentExist<File::Table::ParentTableEntityId>(parentEntityId)) {\
					processParent(GetComponent<File::Table::ParentTableEntityId>(parentEntityId)->id_);\
				}\
				std::string tableName = GetComponent<File::Table::Name>(parentEntityId)->name_;\
				namespaceStrings.push_back(tableName);\
				};\
			if (!beginTableCF.IsSet<File::Table::ParentTableEntityId>()) {\
				return std::string{ "" };\
			}\
			else {\
				ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_;\
				processParent(parentTableEntityId);\
				ASSERT(!namespaceStrings.empty());\
				std::string resultNamespace;\
				for (Common::Index i = 0; i < namespaceStrings.size(); i++) {\
					std::string& string = namespaceStrings[i];\
					if (isUppercase) {\
						/*Change values in namespaceStrings, we will not use it later.*/\
						std::transform(string.begin(), string.end(), string.begin(),\
							[](unsigned char c) { return std::toupper(c); });\
					}\
					resultNamespace += string;\
					if (i != namespaceStrings.size() - 1) {\
						resultNamespace += delimiter;\
					}\
				}\
				return resultNamespace;\
			}\
			}(entityId,delimiterString,isNamespaceUppercase)


#define GET_TABLE_FULL_NAME(entityId, delimiterString, isNamespaceUppercase)\
			[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(tableEntityId));\
				std::string tableName = GetComponent<File::Table::Name>(tableEntityId)->name_;\
				if (isUppercase) {\
					std::transform(tableName.begin(), tableName.end(), tableName.begin(),\
						[](unsigned char c) { return std::toupper(c); });\
				}\
				const std::string tableNamespace = GET_TABLE_NAMESPACE(tableEntityId, delimiter, isUppercase);\
				if (tableNamespace != "") {\
					return tableNamespace + delimiter + tableName;\
				}\
				else {\
					return tableName;\
				}\
			}(entityId, delimiterString, isNamespaceUppercase)


#define GET_TABLE_PARENTS(entityId)\
			[this](ECS2::Entity::Id tableEntityId)->std::vector<ECS2::Entity::Id> {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				if (!IsComponentExist<File::Table::ParentTableEntityId>(tableEntityId)) {\
					return {};\
				}\
				else {\
					ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_;\
					std::vector<ECS2::Entity::Id> parentTables;\
					std::function<void(ECS2::Entity::Id)> processParent =\
						[this, &parentTables, &processParent]\
						(ECS2::Entity::Id parentTableEntityId) {\
						if (IsComponentExist<File::Table::ParentTableEntityId>(parentTableEntityId)) {\
							processParent(GetComponent<File::Table::ParentTableEntityId>(parentTableEntityId)->id_);\
						}\
						parentTables.push_back(parentTableEntityId);\
						};\
					processParent(parentTableEntityId);\
					return parentTables;\
				}\
			}(entityId)

#define CPP_TREE_CREATE_NAMESPACE(namespaceName)\
	[this](const std::string& name){\
			const ECS2::Entity::Id namespaceEntityId = CreateEntity();\
			CreateComponent<CPP::Tree::Node::Tag>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Decl::Tag>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Decl::Namespace_>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Node::Name>(namespaceEntityId, name);\
			return namespaceEntityId;\
			}(namespaceName)

	//If cppTableEntity id has only one parent namespace firstNamespaceEntityId == lastNamespaceEntityId == valid ECS2::Entity::Id
	//If cppTableEntity has no parents firstNamespaceEntityId == lastNamespaceEntityId == ECS2::Entity::Id::invalid_
#define CREATE_CPP_NAMESPACES(parentEntityIds, entityId)\
		[this](const std::vector<ECS2::Entity::Id>& parentTableEntityIds, ECS2::Entity::Id cppTableEntityId) ->std::pair<ECS2::Entity::Id, ECS2::Entity::Id> {\
			ECS2::Entity::Id firstNamespaceEntityId = ECS2::Entity::Id::invalid_;\
			ECS2::Entity::Id lastNamespaceEntityId = ECS2::Entity::Id::invalid_;\
			std::vector<ECS2::Entity::Id> cppNamespaceEntityIds;\
			for (Common::Index i = 0; i < parentTableEntityIds.size(); i++) {\
				ECS2::Entity::Id parentTableEntityId = parentTableEntityIds[i]; \
				const ECS2::Entity::Id namespaceEntityId = CPP_TREE_CREATE_NAMESPACE(GetComponent<File::Table::Name>(parentTableEntityId)->name_);\
				cppNamespaceEntityIds.push_back(namespaceEntityId);\
			}\
			for (Common::Index i = 0; i < parentTableEntityIds.size(); i++) {\
				ECS2::Entity::Id parentTableEntityId = parentTableEntityIds[i];\
				const ECS2::ComponentsFilter tableCF = GetComponentsFilter(parentTableEntityId);\
				ASSERT(tableCF.IsSet<File::Table::Namespace::Tag>());\
				ASSERT(tableCF.IsSet<File::Table::ChildTablesEntityIds>());\
				const ECS2::Entity::Id namespaceEntityId = cppNamespaceEntityIds[i];\
				if (i != parentTableEntityIds.size() - 1) {\
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(namespaceEntityId, std::vector{ cppNamespaceEntityIds[i + 1] });\
				}\
				else {\
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(namespaceEntityId,std::vector{  cppTableEntityId });\
					lastNamespaceEntityId = namespaceEntityId;\
				}\
				if (i != 0) {\
					CreateComponent<CPP::Tree::Node::ParentEntityId>(namespaceEntityId, cppNamespaceEntityIds[i - 1]);\
				}\
				else {\
					firstNamespaceEntityId = namespaceEntityId;\
				}\
			}\
			return { firstNamespaceEntityId, lastNamespaceEntityId };\
		}(parentEntityIds, entityId)

#define CREATE_CPP_VARIABLE(variableTypeName, variableFieldName, variableInitializeValue)\
		[this](const std::string& typeName, const std::string& fieldName, const std::string& initValue) {\
			ECS2::Entity::Id variableEntityId = CreateEntity();\
			CreateComponent<CPP::Tree::Node::Tag>(variableEntityId);\
			{\
				ECS2::Entity::Id typeEntityId = CreateEntity();\
				CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
				CreateComponent<CPP::Tree::Type::Name>(typeEntityId, typeName);\
				ECS2::Entity::Id initializeEntityId = CreateEntity();\
				CreateComponent<CPP::Tree::Expr::Tag>(initializeEntityId);\
				CreateComponent<CPP::Tree::Expr::Literal>(initializeEntityId,\
					Common::Limits<decltype(CPP::Tree::Expr::Literal::kind_)>::Max(),\
					initValue\
				);\
				CreateComponent<CPP::Tree::Decl::Variable>(variableEntityId, typeEntityId, initializeEntityId);\
			}\
			CreateComponent<CPP::Tree::Node::Name>(variableEntityId, fieldName);\
			return variableEntityId;\
		}(variableTypeName, variableFieldName, variableInitializeValue)

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

			ECS2::Entity::Id cppFileEntityId = CreateEntity<CPP__FILE__FILE>();

			CreateComponent<CPP::File::Tag>(cppFileEntityId);
			CreateComponent<CPP::File::Type::Hpp>(cppFileEntityId);

			const auto* path = std::get<ECS::File::Path*>(allFiles) + i;

			//File PATH and NAME
			CreateComponent<CPP::File::Path>(cppFileEntityId, std::filesystem::path{ path->path_ }.parent_path().string());
			CreateComponent<CPP::File::Name>(cppFileEntityId, std::filesystem::path{ path->path_ }.stem().string());

			//File Required INLCUDES
			std::vector<ECS2::Entity::Id> includes;
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("ECS2.hpp"));
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("chrono"));
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("tuple"));
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("OksEngine.IComponent.hpp"));
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("Common.hpp"));
			includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("Common.Bitset.hpp"));

			CreateComponent<CPP::File::Include::EntityIds>(cppFileEntityId, includes);


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
				};


			const auto* fileTables = std::get<ECS::File::Table::EntityIds*>(allFiles) + i;
			for (ECS2::Entity::Id tableEntityId : fileTables->entityIds_) {
				getFileTables(tableEntityId);
			}

			std::vector<ECS2::Entity::Id> fileNodeEntityIds;
			//Generate constants code tree.
			for (ECS2::Entity::Id constantEntityId : fileConstants) {
				const ECS2::ComponentsFilter constantCF = GetComponentsFilter(constantEntityId);
				ASSERT(constantCF.IsSet<File::Table::Constant::Tag>());


				const ECS2::Entity::Id cppConstantEntityId = CreateEntity();
				const auto parentTables = GET_TABLE_PARENTS(constantEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CREATE_CPP_NAMESPACES(parentTables, cppConstantEntityId);
				CreateComponent<CPP::Tree::Node::Tag>(cppConstantEntityId);
				CreateComponent<CPP::Tree::Decl::Tag>(cppConstantEntityId);
				{
					ECS2::Entity::Id typeEntityId = CreateEntity();
					CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);
					CreateComponent<CPP::Tree::Type::Name>(typeEntityId, GetComponent<File::Table::Constant::TypeName>(constantEntityId)->type_);

					ECS2::Entity::Id initializeEntityId = CreateEntity();
					CreateComponent<CPP::Tree::Expr::Tag>(initializeEntityId);
					CreateComponent<CPP::Tree::Expr::Literal>(initializeEntityId,
						Common::Limits<decltype(CPP::Tree::Expr::Literal::kind_)>::Max(),
						GetComponent<File::Table::Constant::Value>(constantEntityId)->value_
					);

					CreateComponent<CPP::Tree::Decl::Variable>(cppConstantEntityId, typeEntityId, initializeEntityId);
				}
				CreateComponent<CPP::Tree::Node::Name>(cppConstantEntityId, GetComponent<File::Table::Name>(constantEntityId)->name_);
				CreateComponent<CPP::Tree::StorageClass::Static_>(cppConstantEntityId);
				CreateComponent<CPP::Tree::Constexpr_>(cppConstantEntityId);
				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppConstantEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					fileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					fileNodeEntityIds.push_back(cppConstantEntityId);
				}
			}
			//Generate structs code tree.
			for (ECS2::Entity::Id structEntityId : fileStructs) {

				const ECS2::ComponentsFilter structCF = GetComponentsFilter(structEntityId);
				ASSERT(structCF.IsSet<File::Table::Struct::Tag>());

				const ECS2::Entity::Id cppStructEntityId = CreateEntity();
				const auto ecsParentTables = GET_TABLE_PARENTS(structEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CREATE_CPP_NAMESPACES(ecsParentTables, cppStructEntityId);

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

							ECS2::Entity::Id variableEntityId = CreateEntity();

							CreateComponent<CPP::Tree::Node::Tag>(variableEntityId);
							CreateComponent<CPP::Tree::Decl::Tag>(variableEntityId);
							//Create type entity.
							ECS2::Entity::Id typeEntityId = CreateEntity();
							CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);
							CreateComponent<CPP::Tree::Type::Name>(typeEntityId, GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_);

							//Field name.
							CreateComponent<CPP::Tree::Node::Name>(variableEntityId,
								GetComponent<File::Table::Struct::Field::Name>(fieldEntityId)->name_ + "_");

							//Create initializer if need.
							ECS2::Entity::Id initializeEntityId = ECS2::Entity::Id::invalid_;
							if (IsComponentExist<File::Table::Struct::Field::DefaultValue>(fieldEntityId)) {
								initializeEntityId = CreateEntity();
								CreateComponent<CPP::Tree::Expr::Tag>(initializeEntityId);
								CreateComponent<CPP::Tree::Expr::Literal>(initializeEntityId,
									Common::Limits<decltype(CPP::Tree::Expr::Literal::kind_)>::Max(),
									GetComponent<File::Table::Struct::Field::DefaultValue>(fieldEntityId)->value_);
							}

							CreateComponent<CPP::Tree::Decl::Variable>(variableEntityId, typeEntityId, initializeEntityId);
							CreateComponent<CPP::Tree::Access::Public_>(variableEntityId);
							//Field entity id -> Struct entity id.
							CreateComponent<CPP::Tree::Node::ParentEntityId>(variableEntityId, cppStructEntityId);

							fieldEntityIds.push_back(variableEntityId);
							childEntityIds.push_back(variableEntityId);
						}
					}
					//Default constructor.
					{
						ECS2::Entity::Id defaultConstructorEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Decl::Function>(defaultConstructorEntityId, ECS2::Entity::Id::invalid_);//No return value for constructor.
						CreateComponent<CPP::Tree::Decl::Constructor>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Node::Name>(defaultConstructorEntityId, GetComponent<File::Table::Name>(structEntityId)->name_);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(defaultConstructorEntityId, cppStructEntityId);
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(defaultConstructorEntityId, std::vector<ECS2::Entity::Id>{}); // No body content.
						childEntityIds.push_back(defaultConstructorEntityId);
					}
					//Constructor with parameters.
					if(!fieldEntityIds.empty()){
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
							ECS2::Entity::Id typeEntityId = CreateEntity();
							{
								CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);
								CreateComponent<CPP::Tree::Type::Name>(typeEntityId, GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_);
							}

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


						CreateComponent<CPP::Tree::Node::ChildEntityIds>(constructorWithParametersEntityId, parameterEntityIds); // No body content.
						childEntityIds.push_back(constructorWithParametersEntityId);

					}
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppStructEntityId, childEntityIds);
				}
				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppStructEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					fileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					fileNodeEntityIds.push_back(cppStructEntityId);
				}
			}

			//Generate components code tree.
			for (ECS2::Entity::Id componentEntityId : fileComponents) {

				const ECS2::ComponentsFilter componentCF = GetComponentsFilter(componentEntityId);
				ASSERT(componentCF.IsSet<File::Table::Component::Tag>());

				const ECS2::Entity::Id cppComponentEntityId = CreateEntity();
				const auto ecsParentTables = GET_TABLE_PARENTS(componentEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CREATE_CPP_NAMESPACES(ecsParentTables, cppComponentEntityId);

				{
					CreateComponent<CPP::Tree::Node::Tag>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Decl::Tag>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Decl::Class_>(cppComponentEntityId);
					CreateComponent<CPP::Tree::Node::Name>(cppComponentEntityId, GetComponent<File::Table::Name>(componentEntityId)->name_);

					std::vector<ECS2::Entity::Id> childEntityIds;
					std::vector<ECS2::Entity::Id> fieldEntityIds;

					if (IsComponentExist<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)) {
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)->entityIds_) {
							ASSERT(IsComponentExist<ECS::File::Table::Component::Field::Tag>(fieldEntityId));

							ECS2::Entity::Id variableEntityId = CreateEntity();

							CreateComponent<CPP::Tree::Node::Tag>(variableEntityId);
							CreateComponent<CPP::Tree::Decl::Tag>(variableEntityId);

							//Create type entity.
							ECS2::Entity::Id typeEntityId = CreateEntity();
							CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);
							CreateComponent<CPP::Tree::Type::Name>(typeEntityId, GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_);

							//Field name.
							CreateComponent<CPP::Tree::Node::Name>(variableEntityId,
								GetComponent<File::Table::Component::Field::Name>(fieldEntityId)->name_ + "_");

							//Create initializer if need.
							ECS2::Entity::Id initializeEntityId = ECS2::Entity::Id::invalid_;
							if (IsComponentExist<File::Table::Component::Field::DefaultValue>(fieldEntityId)) {
								initializeEntityId = CreateEntity();
								CreateComponent<CPP::Tree::Expr::Tag>(initializeEntityId);
								CreateComponent<CPP::Tree::Expr::Literal>(initializeEntityId,
									Common::Limits<decltype(CPP::Tree::Expr::Literal::kind_)>::Max(),
									GetComponent<File::Table::Component::Field::DefaultValue>(fieldEntityId)->value_);
							}

							CreateComponent<CPP::Tree::Decl::Variable>(variableEntityId, typeEntityId, initializeEntityId);
							CreateComponent<CPP::Tree::Access::Public_>(variableEntityId);
							//Field entity id -> Struct entity id.
							CreateComponent<CPP::Tree::Node::ParentEntityId>(variableEntityId, cppComponentEntityId);

							fieldEntityIds.push_back(variableEntityId);
							childEntityIds.push_back(variableEntityId);
						}
					}
					//Default constructor.
					{
						ECS2::Entity::Id defaultConstructorEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Decl::Function>(defaultConstructorEntityId, ECS2::Entity::Id::invalid_);//No return value for constructor.
						CreateComponent<CPP::Tree::Decl::Constructor>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(defaultConstructorEntityId);
						CreateComponent<CPP::Tree::Node::Name>(defaultConstructorEntityId, GetComponent<File::Table::Name>(componentEntityId)->name_);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(defaultConstructorEntityId, cppComponentEntityId);
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(defaultConstructorEntityId, std::vector<ECS2::Entity::Id>{}); // No body content.
						childEntityIds.push_back(defaultConstructorEntityId);
					}
					//Constructor with parameters.
					if(!fieldEntityIds.empty()){
						//Create constructor with parameters only if there are fields.
						ECS2::Entity::Id constructorWithParametersEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Decl::Function>(constructorWithParametersEntityId, ECS2::Entity::Id::invalid_);//No return value for constructor.
						CreateComponent<CPP::Tree::Decl::Constructor>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Access::Public_>(constructorWithParametersEntityId);
						CreateComponent<CPP::Tree::Node::Name>(constructorWithParametersEntityId, GetComponent<File::Table::Name>(componentEntityId)->name_);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(constructorWithParametersEntityId, cppComponentEntityId);

						std::vector<ECS2::Entity::Id> parameterEntityIds;
						if (IsComponentExist<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)) {
							for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Component::Field::EntityIds>(componentEntityId)->entityIds_) {
								GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_;

								ECS2::Entity::Id parameterEntityId = CreateEntity();
								CreateComponent<CPP::Tree::Node::Tag>(parameterEntityId);
								CreateComponent<CPP::Tree::Decl::Tag>(parameterEntityId);

								//Type entity id.
								ECS2::Entity::Id typeEntityId = CreateEntity();
								{
									CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);
									CreateComponent<CPP::Tree::Type::Name>(typeEntityId, GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_);
								}

								CreateComponent<CPP::Tree::Decl::Parameter>(parameterEntityId,
									GetComponent<ECS::File::Table::Component::Field::Name>(fieldEntityId)->name_,
									typeEntityId,
									ECS2::Entity::Id::invalid_ // There is no initializer.
								);
								parameterEntityIds.push_back(parameterEntityId);
							}
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
							if (!initializerEntityIds.empty()) {
								CreateComponent<CPP::Tree::ConstructorInitList>(constructorWithParametersEntityId, initializerEntityIds);
							}
						}

						CreateComponent<CPP::Tree::Node::ChildEntityIds>(constructorWithParametersEntityId, parameterEntityIds); // No body content.
						childEntityIds.push_back(constructorWithParametersEntityId);
					}

					//GetName static Function
					//static const char* GetName()
					//{
					//	return "Config::Tag";
					//};
					{
						ECS2::Entity::Id getNameFunctionEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(getNameFunctionEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(getNameFunctionEntityId);
						CreateComponent<CPP::Tree::StorageClass::Static_>(getNameFunctionEntityId);
						CreateComponent<CPP::Tree::Node::Name>(getNameFunctionEntityId, "GetName");
						//Return value.
						{
							ECS2::Entity::Id returnTypeEntityId = CreateEntity();
							CreateComponent<CPP::Tree::Type::Tag>(returnTypeEntityId);
							CreateComponent<CPP::Tree::Type::Name>(returnTypeEntityId, "const char*");//TODO: replace with PointerType
							CreateComponent<CPP::Tree::Decl::Function>(getNameFunctionEntityId, returnTypeEntityId);
						}
						CreateComponent<CPP::Tree::Access::Public_>(getNameFunctionEntityId);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(getNameFunctionEntityId, cppComponentEntityId);
						//Body.
						ECS2::Entity::Id bodyEntityId = CreateEntity();
						{
							
							CreateComponent<CPP::Tree::Stmt::Tag>(bodyEntityId);
							CreateComponent<CPP::Tree::Stmt::CompoundStmt>(bodyEntityId);

							ECS2::Entity::Id returnStmtEntityId = CreateEntity();
							{
								CreateComponent<CPP::Tree::Stmt::Tag>(returnStmtEntityId);
								ECS2::Entity::Id exprEntityId = CreateEntity();
								{
									CreateComponent<CPP::Tree::Expr::Tag>(exprEntityId);
									CreateComponent<CPP::Tree::Expr::Literal>(exprEntityId,
										Common::Limits<Common::UInt64>::Max(),
										"\"" + GET_TABLE_FULL_NAME(componentEntityId, "::", false) + "\"");
								}
								CreateComponent<CPP::Tree::Stmt::ReturnStmt>(returnStmtEntityId, exprEntityId);
							}
							CreateComponent<CPP::Tree::Node::ChildEntityIds>(bodyEntityId, std::vector<ECS2::Entity::Id>{ returnStmtEntityId  });
						}
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(getNameFunctionEntityId, std::vector<ECS2::Entity::Id>{ bodyEntityId }); // No body content.
						childEntityIds.push_back(getNameFunctionEntityId);
					}
					//GetTypeId static Function
					//static inline ECS2::ComponentTypeId GetTypeId()
					//{
					//	return 211;
					//};
					{
						ECS2::Entity::Id getTypeIdFunctionEntityId = CreateEntity();
						CreateComponent<CPP::Tree::Node::Tag>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::Decl::Tag>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::StorageClass::Static_>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::Inline_>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::Node::Name>(getTypeIdFunctionEntityId, "GetTypeId");

						//Return value.
						{
							ECS2::Entity::Id returnTypeEntityId = CreateEntity();
							CreateComponent<CPP::Tree::Type::Tag>(returnTypeEntityId);
							CreateComponent<CPP::Tree::Type::Name>(returnTypeEntityId, "ECS2::ComponentTypeId");//TODO: replace with PointerType
							CreateComponent<CPP::Tree::Decl::Function>(getTypeIdFunctionEntityId, returnTypeEntityId);
						}
						CreateComponent<CPP::Tree::Access::Public_>(getTypeIdFunctionEntityId);
						CreateComponent<CPP::Tree::Node::ParentEntityId>(getTypeIdFunctionEntityId, cppComponentEntityId);
						//Body.
						ECS2::Entity::Id bodyEntityId = CreateEntity();
						{
							CreateComponent<CPP::Tree::Stmt::Tag>(bodyEntityId);
							CreateComponent<CPP::Tree::Stmt::CompoundStmt>(bodyEntityId);

							ECS2::Entity::Id returnStmtEntityId = CreateEntity();
							{
								CreateComponent<CPP::Tree::Stmt::Tag>(returnStmtEntityId);
								ECS2::Entity::Id exprEntityId = CreateEntity();
								{
									CreateComponent<CPP::Tree::Expr::Tag>(exprEntityId);
									CreateComponent<CPP::Tree::Expr::Literal>(exprEntityId,
										Common::Limits<Common::UInt64>::Max(),
										std::to_string(++componentTypeId));
								}
								CreateComponent<CPP::Tree::Stmt::ReturnStmt>(returnStmtEntityId, exprEntityId);
							}
							CreateComponent<CPP::Tree::Node::ChildEntityIds>(bodyEntityId, std::vector<ECS2::Entity::Id>{ returnStmtEntityId  });
						}
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(getTypeIdFunctionEntityId, std::vector<ECS2::Entity::Id>{ bodyEntityId }); // No body content.
						childEntityIds.push_back(getTypeIdFunctionEntityId);
					}

					CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppComponentEntityId, childEntityIds);
				}
				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppComponentEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					fileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					fileNodeEntityIds.push_back(cppComponentEntityId);
				}
			}

			//Generate Enums code tree.
			for (ECS2::Entity::Id enumEntityId : fileEnums) {
				const ECS2::ComponentsFilter enumCF = GetComponentsFilter(enumEntityId);
				ASSERT(enumCF.IsSet<File::Table::Enum::Tag>());

				const ECS2::Entity::Id cppEnumEntityId = CreateEntity();
				const auto ecsParentTables = GET_TABLE_PARENTS(enumEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CREATE_CPP_NAMESPACES(ecsParentTables, cppEnumEntityId);
				
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
					fileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					fileNodeEntityIds.push_back(cppEnumEntityId);
				}
			}

			//Generate Systems code tree.
			for (ECS2::Entity::Id systemEntityId : fileSystems) {
				const ECS2::ComponentsFilter systemCF = GetComponentsFilter(systemEntityId);
				ASSERT(systemCF.IsSet<File::Table::System::Tag>());

				const ECS2::Entity::Id cppSystemEntityId = CreateEntity();
				const auto ecsParentTables = GET_TABLE_PARENTS(systemEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CREATE_CPP_NAMESPACES(ecsParentTables, cppSystemEntityId);

				CreateComponent<CPP::Tree::Node::Tag>(cppSystemEntityId);
				CreateComponent<CPP::Tree::Decl::Tag>(cppSystemEntityId);
				CreateComponent<CPP::Tree::Decl::Class_>(cppSystemEntityId);
				CreateComponent<CPP::Tree::Node::Name>(cppSystemEntityId, GetComponent<File::Table::Name>(cppSystemEntityId)->name_);

				std::vector<ECS2::Entity::Id> childEntityIds;
				
				//Default constructor.
				{
					ECS2::Entity::Id defaultConstructorEntityId = CreateEntity();
					CreateComponent<CPP::Tree::Node::Tag>(defaultConstructorEntityId);
					CreateComponent<CPP::Tree::Decl::Tag>(defaultConstructorEntityId);
					CreateComponent<CPP::Tree::Decl::Function>(defaultConstructorEntityId, ECS2::Entity::Id::invalid_);//No return value for constructor.
					CreateComponent<CPP::Tree::Decl::Constructor>(defaultConstructorEntityId);
					CreateComponent<CPP::Tree::Access::Public_>(defaultConstructorEntityId);
					CreateComponent<CPP::Tree::Node::Name>(defaultConstructorEntityId, GetComponent<File::Table::Name>(systemEntityId)->name_);
					CreateComponent<CPP::Tree::Node::ParentEntityId>(defaultConstructorEntityId, cppSystemEntityId);
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(defaultConstructorEntityId, std::vector<ECS2::Entity::Id>{}); // No body content.
					childEntityIds.push_back(defaultConstructorEntityId);
				}

				CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppSystemEntityId, childEntityIds);

				if (lastNamespaceEntityId.IsValid()) {
					CreateComponent<CPP::Tree::Node::ParentEntityId>(cppSystemEntityId, lastNamespaceEntityId);
				}
				if (firstNamespaceEntityId.IsValid()) {
					fileNodeEntityIds.push_back(firstNamespaceEntityId);
				}
				else {
					fileNodeEntityIds.push_back(cppSystemEntityId);
				}
			}

			CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppFileEntityId, fileNodeEntityIds);
		}


		const auto allComponents = GetComponents<ECS__FILE__TABLE__COMPONENT__COMPONENT>();
		const Common::Size allComponentsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__COMPONENT__COMPONENT>();

		const auto allSystems = GetComponents<ECS__FILE__TABLE__SYSTEM__SYSTEM>();
		const Common::Size allSystemsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__SYSTEM__SYSTEM>();

	}


}; // namespace OksEngine::ECS::Generator