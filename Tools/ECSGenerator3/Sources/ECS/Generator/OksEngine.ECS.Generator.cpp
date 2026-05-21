#pragma once
#include <ECS\Generator\OksEngine.ECS.Generator.Utils.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <CPP/OksEngine.CPP.Tree.Utils.hpp>
#include <ECS/File/OksEngine.ECS.File.Utils.hpp>
#include <ECS/File/auto_OksEngine.ECS.File.Archetype.hpp>

namespace OksEngine::ECS::Generator
{


	//.ecs file to .hpp file.
	void GenerateTrees::Update(
		ECS2::Entity::Id entity0id,
		const OksEngine::ECS::Project::Tag* eCS__Project__Tag0,
		const OksEngine::ECS::Project::Path* eCS__Project__Path0,
		const OksEngine::LuaScript* luaScript0) {

		const auto projectFilePath = std::filesystem::path{ eCS__Project__Path0->path_ };
		std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(projectFilePath.string());
		::Lua::Context context;
		::Lua::Script script{ std::string{ data.data(), data.size() } };
		context.LoadScript(script);
		luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");
		ASSERT_FMSG(ecsTable.isTable(), "");
		luabridge::LuaRef includesTable = ecsTable["Includes"];

		const auto allFiles = GetComponents<ECS__FILE__FILE>();
		const Common::Size allFilesNumber = world_->GetEntitiesNumber<ECS__FILE__FILE>();

		Common::Size componentTypeId = 0;

		for (Common::Index i = 0; i < allFilesNumber; i++) {


			const auto* path = std::get<ECS::File::Path*>(allFiles) + i;
			ECS2::Entity::Id fileEntityId = *(std::get<ECS2::Entity::Id*>(allFiles) + i);

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
					const std::filesystem::path correspondingHppFileIncludePath = ECS__FILE__GET_FILE_INCLUDE_PATH(fileEntityId, projectFilePath, ".hpp");
					cppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE(correspondingHppFileIncludePath.string()));
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

			{
				std::unordered_set<std::string> requiredIncludes;


				auto getIncludesByTypeName = [&](const std::string& typeName) {
					std::vector<std::string> includes;
					if (includesTable.isTable()) {

						for (luabridge::Iterator it(includesTable); !it.isNil(); ++it) {
							// Каждый элемент таблицы — это LuaRef, который должен быть таблицей
							luabridge::LuaRef ruleTable = it.value();

							if (ruleTable.isTable()) {

								bool needToIncludeHeader = false;
								// Извлекаем массив "Entries"
								luabridge::LuaRef entriesRef = ruleTable["Entries"];
								if (entriesRef.isTable()) {
									for (luabridge::Iterator entIt(entriesRef); !entIt.isNil(); ++entIt) {
										// Значение должно быть строкой
										if (entIt.value().isString()) {
											std::string key = entIt.value().cast<std::string>().value();
											if (typeName.find(key) != std::string::npos) {
												needToIncludeHeader = true;
											}
										}
									}
								}

								// Аналогично извлекаем массив "Headers"
								luabridge::LuaRef headersRef = ruleTable["Headers"];
								if (headersRef.isTable()) {
									for (luabridge::Iterator hdrIt(headersRef); !hdrIt.isNil(); ++hdrIt) {
										if (hdrIt.value().isString() && needToIncludeHeader) {
											includes.push_back(hdrIt.value().cast<std::string>().value());
										}
									}
								}
							}
						}
					}
					return includes;
					};

				for (ECS2::Entity::Id ecsStructEntityId : fileStructs) {
					if (IsComponentExist<ECS::File::Table::Struct::Field::EntityIds>(ecsStructEntityId)) {
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Struct::Field::EntityIds>(ecsStructEntityId)->entityIds_) {
							auto includes = getIncludesByTypeName(GetComponent<File::Table::Struct::Field::Type>(fieldEntityId)->name_);
							for (auto include : includes) {
								requiredIncludes.insert(include);
							}
						}
					}
				}

				for (ECS2::Entity::Id ecsComponentEntityId : fileComponents) {
					if (IsComponentExist<ECS::File::Table::Component::Field::EntityIds>(ecsComponentEntityId)) {
						for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Component::Field::EntityIds>(ecsComponentEntityId)->entityIds_) {
							auto includes = getIncludesByTypeName(GetComponent<File::Table::Component::Field::Type>(fieldEntityId)->name_);
							for (auto include : includes) {
								requiredIncludes.insert(include);
							}

						}
					}
				}

				for (ECS2::Entity::Id ecsConstantEntityId : fileConstants) {
					const std::string typeName = GetComponent<ECS::File::Table::Constant::TypeName>(ecsConstantEntityId)->type_;
					auto includes = getIncludesByTypeName(typeName);
					for (auto include : includes) {
						requiredIncludes.insert(include);
					}
				}

				for (ECS2::Entity::Id systemEntityId : fileSystems) {
					if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
						ECS2::Entity::Id updateMethodEntityId = GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
						if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {

							const auto processEntityIds = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;

							if (!processEntityIds.empty()) {
								Common::Index entityIndex = 0;
								for (ECS2::Entity::Id processEntityId : processEntityIds) {
									auto processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processEntityId)->componentInfos_;

									for (const auto& componentInfo : processComponentInfos) {

										if (componentInfo.name_ == "Test::Dependency") {
											Common::BreakPointLine();
										}
										ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, componentInfo.name_);
										ECS2::Entity::Id ecsFileEntityId = ECS__FILE__TABLE__GET_TABLE_FILE_ENTITY(componentEntityId);
										if (ecsFileEntityId == fileEntityId) {
											continue;
										}
										const std::filesystem::path ecsFileIncludePath = ECS__FILE__GET_FILE_INCLUDE_PATH(ecsFileEntityId, projectFilePath, ".hpp");

										hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE(ecsFileIncludePath.string()));
									}

								}
							}
						}
					}
				}

				for (const auto& include : requiredIncludes) {
					hppFileNodeEntityIds.push_back(CPP__TREE__PREPROCESSOR__CREATE_INCLUDE(include));
				}
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

				GetComponent<File::Table::Constant::TypeName>(constantEntityId)->type_;

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
						ECS2::Entity::Id constructorWithParametersEntityId = CPP__TREE__DECL__CREATE_CONSTRUCTOR(
							GetComponent<File::Table::Name>(structEntityId)->name_,
							[&]() {

								auto ecsStructFields = GetComponent<ECS::File::Table::Struct::Field::EntityIds>(structEntityId)->entityIds_;
								ASSERT(ecsStructFields.size() == fieldEntityIds.size());
								std::vector<ECS2::Entity::Id> initializerEntityIds;
								for (Common::Index i = 0; i < ecsStructFields.size(); i++) {
									ECS2::Entity::Id ecsFieldEntityId = ecsStructFields[i];
									const std::string ecsFieldName = GetComponent<ECS::File::Table::Struct::Field::Name>(ecsFieldEntityId)->name_;
									initializerEntityIds.push_back(CPP__TREE__CREATE_CONSTRUCTOR_INITIALIZER(
										fieldEntityIds[i],
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR(ecsFieldName)
									));
								}
								return initializerEntityIds;
							}(),
								cppStructEntityId,
								[&]() {
								std::vector<ECS2::Entity::Id> parameterEntityIds;
								for (ECS2::Entity::Id fieldEntityId : GetComponent<ECS::File::Table::Struct::Field::EntityIds>(structEntityId)->entityIds_) {
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
								return parameterEntityIds;


								}());
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

					ECS2::Entity::Id constructorWithParametersEntityId = CPP__TREE__DECL__CREATE_CONSTRUCTOR(
						GetComponent<File::Table::Name>(componentEntityId)->name_,
						[&]() {
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
							return initializerEntityIds;
						}(),
							cppComponentEntityId,
							[&]() {
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
							return parameterEntityIds;
							}()
								);

					CreateComponent<CPP::Tree::Access::Public_>(constructorWithParametersEntityId);

					childEntityIds.push_back(constructorWithParametersEntityId);

					//GetName static Function
					//static const char* GetName()
					//{
					//	return "Config::Tag";
					//};
					{
						ECS2::Entity::Id getNameFunctionEntityId = CPP__TREE__DECL__CREATE_PUBLIC_STATIC_METHOD(
							CPP__TREE__TYPE__CREATE_POINTER_TYPE(CPP__TREE__TYPE__CREATE_CONST_CHAR_TYPE()),
							"GetName",
							cppComponentEntityId,
							std::vector<ECS2::Entity::Id>{
							CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(
								std::vector<ECS2::Entity::Id>{
								CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
									CPP__TREE__EXPR__CREATE_LITERAL_EXPR("\"" + ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false) + "\"")
								)
							}
							)
						}
						);
						childEntityIds.push_back(getNameFunctionEntityId);
					}
					//GetTypeId static Function
					//static inline ECS2::ComponentTypeId GetTypeId()
					//{
					//	return 211;
					//};
					{
						ECS2::Entity::Id getTypeIdFunctionEntityId = CPP__TREE__DECL__CREATE_PUBLIC_STATIC_INLINE_METHOD(
							CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::ComponentTypeId"),
							"GetTypeId",
							cppComponentEntityId,
							std::vector<ECS2::Entity::Id>{
							CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(
								std::vector<ECS2::Entity::Id>{
								CPP__TREE__STMT__CREATE_RETURN_STATEMENT(CPP__TREE__EXPR__CREATE_LITERAL_EXPR(std::to_string(++componentTypeId)))
							}
							)
						}
						);
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
						//GetComponentsFilter
						{
							ECS2::Entity::Id getComponentsFilterFunctionEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
								CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::ComponentsFilter"),
								"GetComponentsFilter",
								cppSystemClassEntityId,
								(std::vector<ECS2::Entity::Id>{
								CPP__TREE__DECL__CREATE_PARAMETER(CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"), "entityId"),
									CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
										CPP__TREE__EXPR__CREATE_CALL_EXPR(
											CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
												CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
												"GetEntityComponentsFilter",
												true
											),
											std::vector<ECS2::Entity::Id>{},
											std::vector<ECS2::Entity::Id>{CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("entityId")}
										)
									)
							}));
							systemClassChildEntityIds.push_back(getComponentsFilterFunctionEntityId);
						}

						//Destroy entity.
						//void DestroyEntity(ECS2::Entity::Id entityId)
						//{
						//	world_->DestroyEntity(entityId);
						//};
						{
							ECS2::Entity::Id destroyEntityFunctionEntityId = CPP__TREE__DECL__CREATE_FUNCTION(
								CPP__TREE__TYPE__CREATE_VOID_TYPE(),
								"DestroyEntity",
								cppSystemClassEntityId,
								(std::vector<ECS2::Entity::Id>{
								CPP__TREE__DECL__CREATE_PARAMETER(CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"), "entityId"),
									CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
										CPP__TREE__EXPR__CREATE_CALL_EXPR(
											CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
												CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
												"DestroyEntity",
												true
											),
											std::vector<ECS2::Entity::Id>{},
											std::vector<ECS2::Entity::Id>{CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("entityId")}
										)
									)
							}));
							systemClassChildEntityIds.push_back(destroyEntityFunctionEntityId);
						}

						//IsComponentExist
						//template <class Component> bool IsComponentExist(ECS2::Entity::Id entityId)
						//{
						//	return world_->IsComponentExist<Component>(entityId);
						//};
						{
							ECS2::Entity::Id componentTemplateTypEntityId = CPP__TREE__DECL__CREATE_TEMPLATE_TYPE_PARAMETER("Component", ECS2::Entity::Id::invalid_, false);
							std::vector<ECS2::Entity::Id> childs;

							childs.push_back(CPP__TREE__DECL__CREATE_PARAMETER(CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"), "entityId"));

							childs.push_back(CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
								CPP__TREE__EXPR__CREATE_CALL_EXPR(
									CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
										"IsComponentExist",
										true
									),
									std::vector<ECS2::Entity::Id>{componentTemplateTypEntityId},
									std::vector<ECS2::Entity::Id>{CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("entityId")}
								)
							));

							ECS2::Entity::Id functionEntityId = CreateEntity();

							CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
								functionEntityId,
								CPP__TREE__TYPE__CREATE_BOOL_TYPE(),
								std::vector<ECS2::Entity::Id>{ componentTemplateTypEntityId },
								"IsComponentExist",
								cppSystemClassEntityId,
								childs);

							CreateComponent<CPP::Tree::Access::Public_>(functionEntityId);

							systemClassChildEntityIds.push_back(functionEntityId);
						}
						//IsEntityExist
						//template <class... Components> bool IsEntityExist()
						//{
						//	return world_->IsEntityExist<Components...>();
						//};
						{
							ECS2::Entity::Id componentTemplateTypEntityId = CPP__TREE__DECL__CREATE_TEMPLATE_TYPE_PARAMETER("Components", ECS2::Entity::Id::invalid_, true);

							std::vector<ECS2::Entity::Id> childs;
							childs.push_back(CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
								CPP__TREE__EXPR__CREATE_CALL_EXPR(
									CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
										"IsEntityExist",
										true
									),
									std::vector<ECS2::Entity::Id>{componentTemplateTypEntityId},
									std::vector<ECS2::Entity::Id>{}
								)
							));


							ECS2::Entity::Id functionEntityId = CreateEntity();

							CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
								functionEntityId,
								CPP__TREE__TYPE__CREATE_BOOL_TYPE(),
								std::vector<ECS2::Entity::Id>{ componentTemplateTypEntityId },
								"IsEntityExist",
								cppSystemClassEntityId,
								childs);

							CreateComponent<CPP::Tree::Access::Public_>(functionEntityId);

							systemClassChildEntityIds.push_back(functionEntityId);
						}
						//IsEntityExist
						//bool IsEntityExist(ECS2::Entity::Id entityId)
						//{
						//	return world_->IsEntityExist(entityId);
						//};
						{
							std::vector<ECS2::Entity::Id> childs;
							childs.push_back(CPP__TREE__DECL__CREATE_PARAMETER(CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"), "entityId"));
							childs.push_back(CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
								CPP__TREE__EXPR__CREATE_CALL_EXPR(
									CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
										"IsEntityExist",
										true
									),
									std::vector<ECS2::Entity::Id>{},
									std::vector<ECS2::Entity::Id>{CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("entityId")}
								)
							));


							ECS2::Entity::Id functionEntityId = CreateEntity();

							CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
								functionEntityId,
								CPP__TREE__TYPE__CREATE_BOOL_TYPE(),
								std::vector<ECS2::Entity::Id>{ },
								"IsEntityExist",
								cppSystemClassEntityId,
								childs);

							CreateComponent<CPP::Tree::Access::Public_>(functionEntityId);

							systemClassChildEntityIds.push_back(functionEntityId);
						}

						//GetComponent
						{
							ECS2::Entity::Id componentTemplateTypEntityId = CPP__TREE__DECL__CREATE_TEMPLATE_TYPE_PARAMETER("Component", ECS2::Entity::Id::invalid_, false);
							std::vector<ECS2::Entity::Id> childs;
							if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
								ECS2::Entity::Id updateMethodEntityId = GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
								if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {

									const auto processEntityIds = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;

									if (!processEntityIds.empty()) {
										std::vector<ECS2::Entity::Id> isAnyOfArgs;
										isAnyOfArgs.push_back(componentTemplateTypEntityId);
										Common::Index entityIndex = 0;
										for (ECS2::Entity::Id processEntityId : processEntityIds) {
											auto processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processEntityId)->componentInfos_;

											for (const auto& componentInfo : processComponentInfos) {
												ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, componentInfo.name_);
												const std::string componentName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
												isAnyOfArgs.push_back(CPP__TREE__TYPE__CREATE_NAMED_TYPE(componentName));
											}
										}
										ECS2::Entity::Id templateIdExprEntityId = CreateEntity();
										CreateComponent<CPP::Tree::Expr::Tag>(templateIdExprEntityId);
										CreateComponent<CPP::Tree::Expr::TemplateIdExpr>(templateIdExprEntityId, "Common::IsAnyOf", isAnyOfArgs);

										std::vector<ECS2::Entity::Id> macroArgEntityId{
											templateIdExprEntityId,
												CPP__TREE__EXPR__CREATE_LITERAL_EXPR(
													"\"Attempt to access component{} that system(GenerateCode) can't access. Added access entities description "
													"to .ecs file that corresponds to system\"")
										};

										ECS2::Entity::Id macroCallEntityId = CPP__TREE__EXPR__CREATE_MACRO_INVOCATION("STATIC_ASSERT",
											macroArgEntityId, ECS2::Entity::Id::invalid_);

										childs.push_back(CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(std::vector<ECS2::Entity::Id>{CPP__TREE__STMT__CREATE_EXPRESSION_STATEMENT(macroCallEntityId)}));
									}
								}
							}

							childs.push_back(CPP__TREE__STMT__CREATE_RETURN_STATEMENT(
								CPP__TREE__EXPR__CREATE_CALL_EXPR(
									CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(
										CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR("world_"),
										"GetComponent",
										true
									),
									std::vector<ECS2::Entity::Id>{componentTemplateTypEntityId},
									std::vector<ECS2::Entity::Id>{}
								)
							));


							ECS2::Entity::Id functionEntityId = CreateEntity();

							CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(
								functionEntityId,
								CPP__TREE__TYPE__CREATE_POINTER_TYPE(componentTemplateTypEntityId),
								std::vector<ECS2::Entity::Id>{ componentTemplateTypEntityId },
								"GetComponent",
								cppSystemClassEntityId,
								childs);

							CreateComponent<CPP::Tree::Access::Public_>(functionEntityId);

							systemClassChildEntityIds.push_back(functionEntityId);
						}
						//Update method prototype generation.
						{
							std::vector<ECS2::Entity::Id> updateCallParametersEntityIds;
							if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
								ECS2::Entity::Id updateMethodEntityId = GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
								if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {
									const auto processEntityIds = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;

									Common::Index entityIndex = 0;
									for (ECS2::Entity::Id processEntityId : processEntityIds) {
										auto processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processEntityId)->componentInfos_;
										updateCallParametersEntityIds.push_back(CPP__TREE__DECL__CREATE_PARAMETER(
											CPP__TREE__TYPE__CREATE_NAMED_TYPE("ECS2::Entity::Id"),
											"entity" + std::to_string(entityIndex) + "Id")
										);

										for (const auto& componentInfo : processComponentInfos) {
											ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, componentInfo.name_);
											const std::string componentTypeFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
											std::string componentParameterFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "__", true);
											componentParameterFullName = std::string{ (char)std::tolower(componentParameterFullName[0]) } + componentParameterFullName.substr(1);

											ECS2::Entity::Id componentTypeEntityId = CPP__TREE__TYPE__CREATE_NAMED_TYPE(componentTypeFullName);
											if (componentInfo.readonly_) {
												CreateComponent<CPP::Tree::Type::ConstType>(componentTypeEntityId);
											}


											updateCallParametersEntityIds.push_back(CPP__TREE__DECL__CREATE_PARAMETER(
												CPP__TREE__TYPE__CREATE_POINTER_TYPE(componentTypeEntityId),
												componentParameterFullName
											));
										}
										entityIndex++;
									}

								}
							}

							systemClassChildEntityIds.push_back(
								CPP__TREE__DECL__CREATE_FUNCTION(
									CPP__TREE__TYPE__CREATE_VOID_TYPE(),
									"Update",
									cppSystemClassEntityId,
									updateCallParametersEntityIds));
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
							std::vector<ECS2::Entity::Id>{ },
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
											ECS2::Entity::Id excludeComponentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, excludeComponent);
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
											ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, componentInfo.name_);
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
										ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, systemEntityId, componentInfo.name_);
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
						std::vector<ECS2::Entity::Id>{ },
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

				}

			}
			END_PROFILE();

			//Generate archetypes macroses
			for (ECS2::Entity::Id archetypeEntityId : fileArchetypes) {

				const ECS2::ComponentsFilter archetypeCF = GetComponentsFilter(archetypeEntityId);
				ASSERT(archetypeCF.IsSet<File::Table::Archetype::Tag>());

				std::vector<ECS2::Entity::Id> componentEntityIds;
				if (archetypeCF.IsSet<ECS::File::Table::Archetype::Components>()) {
					auto* components = GetComponent<ECS::File::Table::Archetype::Components>(archetypeEntityId);
					for (std::string component : components->components_) {
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Component::Tag, archetypeEntityId, component);
						componentEntityIds.push_back(componentEntityId);
					}
				}

				std::vector<ECS2::Entity::Id> archetypeEntityIds;
				if (archetypeCF.IsSet<ECS::File::Table::Archetype::Archetypes>()) {
					auto* archetypes = GetComponent<ECS::File::Table::Archetype::Archetypes>(archetypeEntityId);
					for (std::string archetype : archetypes->archetypes_) {
						ECS2::Entity::Id refArchetypeEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(ECS::File::Table::Archetype::Tag, archetypeEntityId, archetype);
						archetypeEntityIds.push_back(refArchetypeEntityId);
					}
				}

				const std::string componentFullName = ECS__FILE__TABLE__GET_FULL_NAME(archetypeEntityId, "::", false);

				const ECS2::Entity::Id cppArchetypeEntityId = CreateEntity();
				const auto ecsParentTables = ECS__FILE__TABLE__GET_TABLE_PARENTS(archetypeEntityId);
				const auto [firstNamespaceEntityId, lastNamespaceEntityId] = CPP__TREE__CREATE_CPP_NAMESPACES(
					ecsParentTables,
					std::vector<ECS2::Entity::Id>{cppArchetypeEntityId});
				
				const std::string namespaceStr = ECS__FILE__TABLE__GET_FULL_NAME(archetypeEntityId, "__", true);

				std::string macrosBody;
				bool isFirst = true;
				for (ECS2::Entity::Id componentEntityId : componentEntityIds) {
					const std::string componentFullName = ECS__FILE__TABLE__GET_FULL_NAME(componentEntityId, "::", false);
					if (!isFirst) {
						macrosBody += ",";
					}
					isFirst = false;
					macrosBody += componentFullName;
				}
				for (ECS2::Entity::Id archetypeEntityId : archetypeEntityIds) {
					const std::string archetypeFullName = ECS__FILE__TABLE__GET_FULL_NAME(archetypeEntityId, "__", true);
					if (!isFirst) {
						macrosBody += ",";
					}
					isFirst = false;
					macrosBody += archetypeFullName;
				}


				ECS2::Entity::Id defineEntityId = CreateEntity();
				CreateComponent<CPP::Tree::Preprocessor::Tag>(defineEntityId);
				CreateComponent<CPP::Tree::Preprocessor::Define_>(defineEntityId, namespaceStr, std::vector<std::string>{}, macrosBody, ECS2::Entity::Id::invalid_, false);

				hppFileNodeEntityIds.push_back(defineEntityId);

				//if (firstNamespaceEntityId.IsInvalid()) {
				//	cppFileNodeEntityIds.push_back(runSystemFunctionRealizationEntityId);
				//}

				//if (firstNamespaceEntityId.IsValid()) {
				//	cppFileNodeEntityIds.push_back(firstNamespaceEntityId);
				//}
			}


			CreateComponent<CPP::Tree::Node::ChildEntityIds>(hppFileEntityId, hppFileNodeEntityIds);
			CreateComponent<CPP::Tree::Node::ChildEntityIds>(cppFileEntityId, cppFileNodeEntityIds);
		}


		const auto allComponents = GetComponents<ECS__FILE__TABLE__COMPONENT__COMPONENT>();
		const Common::Size allComponentsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__COMPONENT__COMPONENT>();

		const auto allSystems = GetComponents<ECS__FILE__TABLE__SYSTEM__SYSTEM>();
		const Common::Size allSystemsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__SYSTEM__SYSTEM>();

	}


}; // namespace OksEngine::ECS::Generator