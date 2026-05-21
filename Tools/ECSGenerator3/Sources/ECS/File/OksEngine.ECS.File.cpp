#pragma once

#include <ECS\File\auto_OksEngine.ECS.File.hpp>
#include <ECS\File\OksEngine.ECS.File.Utils.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

namespace OksEngine::ECS::File {

	void Load::Update(
		ECS2::Entity::Id entity0id,
		const OksEngine::CommandLineParameters* commandLineParameters0,
		const OksEngine::ECS::ProjectFilePath* projectFilePath0) {

		//const auto projectFilePath = std::filesystem::path{ projectFilePath0->path_ };

		//std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(projectFilePath.string());

		//::Lua::Context context;
		//::Lua::Script script{ std::string{ data.data(), data.size() } };
		//context.LoadScript(script);

		//luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");

		//ASSERT_FMSG(ecsTable.isTable() || ecsTable["Modules"].isTable(), "");

		//luabridge::LuaRef modulesTable = ecsTable["Modules"];

		//std::vector<std::filesystem::path> modulePaths;

		//int len = modulesTable.length();
		//for (int i = 1; i <= len; ++i) {
		//	if (modulesTable[i].isString()) {
		//		std::filesystem::path modulePath = projectFilePath.parent_path() / modulesTable[i].cast<std::string>().value();
		//		modulePaths.push_back(modulePath.lexically_normal());
		//	}
		//}

		//for (Common::Index i = 0; i < modulePaths.size(); i++) {
		//	const std::filesystem::path modulePath = modulePaths[i];
		//	for (const auto& entry : std::filesystem::recursive_directory_iterator(modulePath)) {
		//		if (std::filesystem::is_regular_file(entry)) {
		//			if (entry.path().extension().string() == ".ecs") {

		//				std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(entry.path().string());

		//				const ECS2::Entity::Id fileEntityId = CreateEntity<ECS__FILE__FILE>();
		//				CreateComponent<ECS::File::Tag>(fileEntityId);
		//				CreateComponent<ECS::File::Path>(fileEntityId, entry.path().string());
		//				CreateComponent<LuaScript>(fileEntityId, std::string{ data.data(), data.size() });
		//			}
		//		}
		//	}
		//}
	};

	namespace Table {
		void Parse::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::ECS::File::Tag* eCS__File__Tag0,
			const OksEngine::ECS::File::Path* eCS__File__Path0,
			const OksEngine::LuaScript* luaScript0) {

			::Lua::Script script{ luaScript0->text_ };

			::Lua::Context context;
			context.LoadScript(script);
			luabridge::LuaRef ecsFile = luabridge::getGlobal(context.state_, "_G");

			std::vector<std::string> abstractionsOrder;
			// Get ECS abstraction order from .lua script source.
			//TODO: Take into account namespaces and their heir
			std::vector<ECS2::Entity::Id> parsedTables;
			{

				auto processLine = [&](const std::string& line) {
					std::smatch match;
					std::regex systemRegex{ R"(\w+System\s*=\s*\{)" };
					std::regex componentRegex{ R"(\w+Component\s*=\s*\{)" };
					std::regex structRegex{ R"(\w+Struct\s*=\s*\{)" };
					std::regex constantRegex{ R"(\w+Constant\s*=\s*\{)" };
					std::regex archetypeRegex{ R"(\w+Archetype\s*=\s*\{)" };
					// »щем System
					if (std::regex_search(line, match, systemRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("=")); // ”бираем "= {"
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
						name.erase(name.size() - sizeof("System") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// »щем Component
					else if (std::regex_search(line, match, componentRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
						name.erase(name.size() - sizeof("Component") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// »щем Struct
					else if (std::regex_search(line, match, structRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Struct") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// »щем Constant
					else if (std::regex_search(line, match, constantRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Constant") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// »щем Archetype
					else if (std::regex_search(line, match, archetypeRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Archetype") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					};
				std::string line;
				std::stringstream stream{ luaScript0->text_ };
				while (std::getline(stream, line)) {
					processLine(line);
				}



				std::function<ECS2::Entity::Id(const std::string&, luabridge::LuaRef)> processTable
					= [&](
						const std::string& name,
						luabridge::LuaRef table) {

							if (name.ends_with("Namespace")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id namespaceTableEntityId = CreateEntity<ECS__FILE__TABLE__NAMESPACE__NAMESPACE>();
								const std::string namespaceName = name.substr(0, name.rfind("Namespace"));

								CreateComponent<ECS::File::Table::Tag>(namespaceTableEntityId);
								CreateComponent<ECS::File::Table::Namespace::Tag>(namespaceTableEntityId);
								CreateComponent<ECS::File::Table::Name>(namespaceTableEntityId, namespaceName);

								std::vector<ECS2::Entity::Id> parsedNamespaceTables;

								for (auto it = luabridge::Iterator(table); !it.isNil(); ++it) {
									luabridge::LuaRef key = it.key();
									luabridge::LuaRef value = it.value();

									if (value.isTable() ||
										value.isString()/*For aliases*/) {
										if (key.isString()) {
											const std::string tableName = key.tostring();
											//TODO: move setting of child table to processTable function.
											ECS2::Entity::Id parsedTable = processTable(tableName, value);
											if (parsedTable.IsValid()) {
												CreateComponent<ECS::File::Table::ParentTableEntityId>(parsedTable, namespaceTableEntityId);
												CreateComponent<File::EntityId>(parsedTable, entity0id);
												parsedNamespaceTables.push_back(parsedTable);
											}

										}
									}
								}
								CreateComponent<ECS::File::Table::ChildTablesEntityIds>(
									namespaceTableEntityId,
									parsedNamespaceTables);

								return namespaceTableEntityId;
							}
							else if (name.ends_with("Struct")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id structEntityId = [this](luabridge::LuaRef component,
									const std::string& structName) {


										ECS2::Entity::Id structEntityId = CreateEntity<ECS__FILE__TABLE__STRUCT__STRUCT>();
										CreateComponent<Table::Tag>(structEntityId);
										CreateComponent<Table::Struct::Tag>(structEntityId);
										CreateComponent<Table::Name>(structEntityId, structName.substr(0, structName.rfind("Struct")));

										Common::Size alignment = Common::Limits<Common::Size>::Max();
										luabridge::LuaRef alignmentRef = component["alignment"];
										if (!alignmentRef.isNil()) {
											alignment = alignmentRef.cast<Common::Size>().value();
											if (alignment != Common::Limits<Common::Size>::Max()) {
												CreateComponent<Table::Struct::Alignment>(structEntityId, alignment);
											}
										}
										luabridge::LuaRef fields = component["fields"];


										if (!fields.isNil()) {
											std::vector<ECS2::Entity::Id> parsedFieldsEntityIds;
											std::vector<std::pair<std::string, std::string>> tablesNames;
											for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
												luabridge::LuaRef field = it.value();

												ECS2::Entity::Id fieldEntityId = CreateEntity<ECS__FILE__TABLE__STRUCT__FIELD__FIELD>();
												CreateComponent<Struct::Field::Tag>(fieldEntityId);
												CreateComponent<Struct::Field::Type>(fieldEntityId, field["type"].cast<std::string>().value());
												CreateComponent<Struct::Field::Name>(fieldEntityId, field["name"].cast<std::string>().value());

												if (!field["defaultValue"].isNil()) {
													CreateComponent<Struct::Field::DefaultValue>(fieldEntityId, field["defaultValue"].cast<std::string>().value());
												}

												if (!field["alignment"].isNil()) {
													CreateComponent<Struct::Field::Alignment>(fieldEntityId, field["alignment"].cast<Common::Size>().value());
												}

												if (!field["copyable"].isNil()) {
													CreateComponent<Struct::Field::Alignment>(fieldEntityId, field["copyable"].cast<bool>().value());
												}

												parsedFieldsEntityIds.push_back(fieldEntityId);
											}
											CreateComponent<Struct::Field::EntityIds>(structEntityId, parsedFieldsEntityIds);
										}

										return structEntityId;
									}(table, name);

								return structEntityId;

							}
							else if (name.ends_with("Enum")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id enumEntityId = [this](luabridge::LuaRef enumRef,
									const std::string& enumName) {

										ECS2::Entity::Id enumEntityId = CreateEntity<ECS__FILE__TABLE__ENUM__ENUM>();
										CreateComponent<Table::Enum::Tag>(enumEntityId);
										CreateComponent<Table::Tag>(enumEntityId);
										CreateComponent<Table::Name>(enumEntityId, enumName.substr(0, enumName.rfind("Struct")));

										luabridge::LuaRef values = enumRef["values"];

										std::vector<std::string> parsedValues;

										if (!values.isNil()) {
											for (luabridge::Iterator it(values); !it.isNil(); ++it) {
												luabridge::LuaRef valueRef = it.value();
												parsedValues.push_back(valueRef.cast<std::string>().value());
											}
										}
										CreateComponent<Table::Enum::Values>(enumEntityId, parsedValues);
										return enumEntityId;
									}(table, name);

								return enumEntityId;
							}
							else if (name.ends_with("Component")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id componentEntityId = [this](luabridge::LuaRef component,
									const std::string& componentName) {

										ASSERT_FMSG(std::isupper(componentName[0]), "First component name symbol must be uppercase.");

										ECS2::Entity::Id componentEntityId = CreateEntity<ECS__FILE__TABLE__COMPONENT__COMPONENT>();
										CreateComponent<Table::Tag>(componentEntityId);
										CreateComponent<Table::Component::Tag>(componentEntityId);
										CreateComponent<Table::Name>(componentEntityId, componentName.substr(0, componentName.rfind("Component")));


										bool isString = component.isString();
										auto castResult = component.cast<std::string>();
										if (castResult) {
											std::string aliasFor = castResult.value();
											CreateComponent<Table::Component::AliesFor>(componentEntityId, aliasFor);
										}
										bool serializable = true;
										luabridge::LuaRef serializableRef = component["serializable"];
										if (!serializableRef.isNil()) {
											serializable = serializableRef.cast<bool>().value();
											if (serializable) {
												CreateComponent<Table::Component::Serializable>(componentEntityId);
											}
										}
										bool bindable = true;
										luabridge::LuaRef bindableRef = component["bindable"];
										if (!bindableRef.isNil()) {
											bindable = bindableRef.cast<bool>().value();
											if (bindable) {
												CreateComponent<Table::Component::Bindable>(componentEntityId);
											}
										}
										Common::Size alignment = Common::Limits<Common::Size>::Max();
										luabridge::LuaRef alignmentRef = component["alignment"];
										if (!alignmentRef.isNil()) {
											alignment = alignmentRef.cast<Common::Size>().value();
											if (alignment != Common::Limits<Common::Size>::Max()) {
												CreateComponent<Table::Component::Alignment>(componentEntityId, alignment);
											}
										}
										bool manualEditFunction = false;
										luabridge::LuaRef manualEditFunctionRef = component["manualEditFunction"];
										if (!manualEditFunctionRef.isNil()) {
											manualEditFunction = manualEditFunctionRef.cast<bool>().value();
											if (manualEditFunction) {
												CreateComponent<Table::Component::ManualEditFunction>(componentEntityId);
											}
										}
										bool manualBindFunction = false;
										luabridge::LuaRef manualBindFunctionRef = component["manualBindFunction"];
										if (!manualBindFunctionRef.isNil()) {
											manualBindFunction = manualBindFunctionRef.cast<bool>().value();
											if (manualBindFunction) {
												CreateComponent<Table::Component::ManualBindFunction>(componentEntityId);
											}
										}
										bool manualAddFunction = false;
										luabridge::LuaRef manualAddFunctionRef = component["manualAddFunction"];
										if (!manualAddFunctionRef.isNil()) {
											manualAddFunction = manualAddFunctionRef.cast<bool>().value();
											if (manualAddFunction) {
												CreateComponent<Table::Component::ManualAddFunction>(componentEntityId);
											}
										}
										bool manualParseFunction = false;
										luabridge::LuaRef manualParseFunctionRef = component["manualParseFunction"];
										if (!manualParseFunctionRef.isNil()) {
											manualParseFunction = manualParseFunctionRef.cast<bool>().value();
											if (manualParseFunction) {
												CreateComponent<Table::Component::ManualParseFunction>(componentEntityId);
											}
										}
										bool manualSerializeFunction = false;
										luabridge::LuaRef manualSerializeFunctionRef = component["manualSerializeFunction"];
										if (!manualSerializeFunctionRef.isNil()) {
											manualSerializeFunction = manualSerializeFunctionRef.cast<bool>().value();
											if (manualSerializeFunction) {
												CreateComponent<Table::Component::ManualSerializeFunction>(componentEntityId);
											}
										}

										luabridge::LuaRef fields = component["fields"];


										if (!fields.isNil()) {
											std::vector<ECS2::Entity::Id> parsedFieldsEntityIds;
											std::vector<std::pair<std::string, std::string>> tablesNames;
											for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
												luabridge::LuaRef field = it.value();

												ECS2::Entity::Id fieldEntityId = CreateEntity<ECS__FILE__TABLE__COMPONENT__FIELD__FIELD>();
												CreateComponent<Component::Field::Tag>(fieldEntityId);
												CreateComponent<Component::Field::Type>(fieldEntityId, field["type"].cast<std::string>().value());
												CreateComponent<Component::Field::Name>(fieldEntityId, field["name"].cast<std::string>().value());

												if (!field["defaultValue"].isNil()) {
													CreateComponent<Component::Field::DefaultValue>(fieldEntityId, field["defaultValue"].cast<std::string>().value());
												}

												if (!field["alignment"].isNil()) {
													CreateComponent<Component::Field::Alignment>(fieldEntityId, field["alignment"].cast<Common::Size>().value());
												}

												if (!field["copyable"].isNil()) {
													CreateComponent<Component::Field::Alignment>(fieldEntityId, field["copyable"].cast<bool>().value());
												}

												parsedFieldsEntityIds.push_back(fieldEntityId);
											}
											CreateComponent<Component::Field::EntityIds>(componentEntityId, parsedFieldsEntityIds);
										}
										if (!bindable) {
											if (manualBindFunction) {
												ASSERT_FAIL_MSG("Attempt to use \"manualBindFunction == true\" option when bindable == false.");
											}
										}

										return componentEntityId;
									}(table, name);

								return componentEntityId;
							}
							else if (name.ends_with("Constant")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								const ECS2::Entity::Id constantEntityId = ECS__FILE__TABLE__PARSE_CONSTANT(table, name.substr(0, name.rfind("Constant")));
								return constantEntityId;
							}
							else if (name.ends_with("Archetype")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id archetypeEntityId = [this](luabridge::LuaRef archetype,
									const std::string& archetypeName) {

										ECS2::Entity::Id archetypeEntityId = CreateEntity<ECS__FILE__TABLE__ARCHETYPE__ARCHETYPE>();
										CreateComponent<ECS::File::Table::Tag>(archetypeEntityId);
										CreateComponent<ECS::File::Table::Archetype::Tag>(archetypeEntityId);
										luabridge::LuaRef components = archetype["components"];
										CreateComponent<Table::Name>(archetypeEntityId, archetypeName.substr(0, archetypeName.rfind("Archetype")));

										if (!components.isNil()) {
											std::vector<std::string> archetypeComponents;
											for (luabridge::Iterator itJ(components); !itJ.isNil(); ++itJ) {
												luabridge::LuaRef componentRef = itJ.value();
												archetypeComponents.push_back({ componentRef.cast<std::string>().value() });
											}
											CreateComponent<Table::Archetype::Components>(
												archetypeEntityId,
												std::vector<ECS2::Entity::Id>{ ECS2::Entity::Id::invalid_, archetypeComponents.size() },
												archetypeComponents);
										}

										luabridge::LuaRef archetypes = archetype["archetypes"];

										if (!archetypes.isNil()) {
											std::vector<std::string> archetypeRefArchetypes;
											for (luabridge::Iterator itJ(archetypes); !itJ.isNil(); ++itJ) {
												luabridge::LuaRef archetypeRef = itJ.value();
												archetypeRefArchetypes.push_back(
													{
														archetypeRef.cast<std::string>().value()
													});
											}
											CreateComponent<Table::Archetype::Archetypes>(
												archetypeEntityId,
												std::vector<ECS2::Entity::Id>{ ECS2::Entity::Id::invalid_, archetypeRefArchetypes.size() },
												archetypeRefArchetypes);
										}
										return archetypeEntityId;

									}(table, name);

								return archetypeEntityId;
							}
							else if (name.ends_with("System")) {
								ASSERT_FMSG(std::isupper(name[0]), "Table name symbol must be uppercase.");
								ECS2::Entity::Id systemEntityId = [this](
									luabridge::LuaRef system,
									const std::string& systemName) {

										ECS2::Entity::Id systemEntityId = CreateEntity<ECS__FILE__TABLE__SYSTEM__SYSTEM>();

										CreateComponent<Table::Tag>(systemEntityId);
										CreateComponent<Table::System::Tag>(systemEntityId);
										CreateComponent<Table::Name>(systemEntityId, systemName.substr(0, systemName.rfind("System")));


										if (system["disable"].cast<bool>().value()) {
											CreateComponent<Table::System::Disabled>(systemEntityId);
										}

										//Parse Call Order.
										[&](luabridge::LuaRef luaRef) {
											luabridge::LuaRef callOrderRef = luaRef["callOrder"];
											if (!callOrderRef.isNil()) {

												ECS2::Entity::Id callOrderEntityId = CreateEntity();

												//Parse runAfter
												[&](luabridge::LuaRef systemRef) {
													std::vector<System::CallOrder::SystemInfo> runAfterSystems;
													luabridge::LuaRef runAfterRef = systemRef["runAfter"];
													if (!runAfterRef.isNil()) {
														if (runAfterRef.isTable()) {
															for (luabridge::Iterator it(runAfterRef); !it.isNil(); ++it) {
																luabridge::LuaRef runAfterSystemRef = it.value();
																runAfterSystems.push_back({ ECS2::Entity::Id::invalid_, runAfterSystemRef.cast<std::string>().value() });
															}
														}
														else {
															runAfterSystems.push_back({ ECS2::Entity::Id::invalid_, runAfterRef.cast<std::string>().value() });
														}
														CreateComponent<System::CallOrder::RunAfter>(callOrderEntityId, runAfterSystems);
													}
													}(callOrderRef);

												//Parse runBefore
												[&](luabridge::LuaRef systemRef) {
													std::vector<System::CallOrder::SystemInfo> runBeforeSystems;
													luabridge::LuaRef runBeforeRef = systemRef["runBefore"];
													if (!runBeforeRef.isNil()) {
														if (runBeforeRef.isTable()) {
															for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
																luabridge::LuaRef runAfterSystemRef = it.value();
																runBeforeSystems.push_back({ ECS2::Entity::Id::invalid_, runAfterSystemRef.cast<std::string>().value() });
															}
														}
														else {
															runBeforeSystems.push_back({ ECS2::Entity::Id::invalid_,runBeforeRef.cast<std::string>().value() });
														}
														CreateComponent<System::CallOrder::RunBefore>(callOrderEntityId, runBeforeSystems);
													}

													}(callOrderRef);

												CreateComponent<System::CallOrder::EntityId>(systemEntityId, callOrderEntityId);
											}
											}(system);

										luabridge::LuaRef updateMethodRef = system["updateMethod"];
										if (!updateMethodRef.isNil()) {

											//Parse UpdateMethod.
											[&](luabridge::LuaRef luaRef) {

												ECS2::Entity::Id updateMethodEntityId = CreateEntity();

												//Parse Accessing Entities.
												[&](luabridge::LuaRef systemRef) {
													//Access entities by id.
													luabridge::LuaRef accessingEntities = systemRef["accessingEntities"];
													if (!accessingEntities.isNil()) {
														std::vector<ECS2::Entity::Id> accessingEntityIds;
														for (luabridge::Iterator it(accessingEntities); !it.isNil(); ++it) {

															ECS2::Entity::Id accessEntityId = CreateEntity<ECS__FILE__TABLE__SYSTEM__UPDATEMETHOD__ACCESS__ENTITY__ENTITY>();
															CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::Tag>(accessEntityId);
															std::string archetype;
															luabridge::LuaRef toAccess = it.value();
															luabridge::LuaRef accessingComponentsRef = toAccess["accessingComponents"];

															//Parse accessingComponents
															if (!accessingComponentsRef.isNil()) {
																std::vector<System::UpdateMethod::Access::Entity::ComponentInfo> accessesComponents;
																for (luabridge::Iterator itJ(accessingComponentsRef); !itJ.isNil(); ++itJ) {
																	luabridge::LuaRef accessingComponentRef = itJ.value();

																	accessesComponents.push_back({
																		accessingComponentRef.cast<std::string>().value(),
																		false
																		});
																	ASSERT_FMSG(std::isupper(accessesComponents.back().name_[0]), "");
																}
																if (!accessesComponents.empty()) {
																	CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(
																		accessEntityId,
																		accessesComponents);
																}
															}

															//Parse archetype
															luabridge::LuaRef archetypeRef = toAccess["archetype"];
															if (!archetypeRef.isNil()) {
																archetype = archetypeRef.cast<std::string>().value();
																CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::Archetype>(
																	accessEntityId,
																	ECS2::Entity::Id::invalid_,
																	archetype
																);
															}

															ASSERT_MSG(
																(!accessingComponentsRef.isNil() && archetypeRef.isNil()) ||
																(accessingComponentsRef.isNil() && !archetype.empty()),
																"Access entity can't access archetype components and raw components at one time.");

															//Parse createsComponents
															luabridge::LuaRef createsComponentsRef = toAccess["createsComponents"];
															if (!createsComponentsRef.isNil()) {
																std::vector<std::string> createsComponents;
																for (luabridge::Iterator itJ(createsComponentsRef); !itJ.isNil(); ++itJ) {
																	luabridge::LuaRef createsComponentRef = itJ.value();
																	createsComponents.push_back({ createsComponentRef.cast<std::string>().value() });
																	ASSERT_FMSG(std::isupper(createsComponents.back()[0]), "");
																}
																if (!createsComponents.empty()) {
																	CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::CreateComponents>(
																		accessEntityId,
																		createsComponents);
																}
															}

															//Parse removesComponents
															luabridge::LuaRef removesComponentsRef = toAccess["removesComponents"];
															if (!removesComponentsRef.isNil()) {
																std::vector<std::string> removesComponents;
																for (luabridge::Iterator itJ(removesComponentsRef); !itJ.isNil(); ++itJ) {
																	luabridge::LuaRef removesComponentRef = itJ.value();
																	removesComponents.push_back(removesComponentRef.cast<std::string>().value());
																	ASSERT_FMSG(std::isupper(removesComponents.back()[0]), "");
																}
																if (!removesComponents.empty()) {
																	CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::RemoveComponents>(
																		accessEntityId,
																		removesComponents);
																}
															}
														}
														CreateComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(
															updateMethodEntityId,
															accessingEntityIds);
													}

													}(luaRef);

												//Parse Processing Entities.
												[&](luabridge::LuaRef luaRef) {

													luabridge::LuaRef entities = luaRef["processesEntities"];
													if (entities.isNil()) {
														return;
													}
													std::vector<ECS2::Entity::Id> proccessesEntities;
													for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
														luabridge::LuaRef entity = it.value();

														ECS2::Entity::Id proccessEntityId = CreateEntity<ECS__FILE__TABLE__SYSTEM__UPDATEMETHOD__PROCESS__ENTITY__ENTITY>();
														CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::Tag>(proccessEntityId);

														luabridge::LuaRef processesAllCombinations = entity["processesAllCombinations"];

														//Parse processesComponents.
														[&](luabridge::LuaRef luaRef) {

															luabridge::LuaRef entityIncludes = luaRef["processesComponents"];

															if (entityIncludes.isNil()) {
																return;
															}

															std::vector<ECS::File::Table::System::UpdateMethod::Process::Entity::ComponentInfo> processesComponents;

															for (luabridge::Iterator it(entityIncludes); !it.isNil(); ++it) {
																ECS::File::Table::System::UpdateMethod::Process::Entity::ComponentInfo componentInfo;
																luabridge::LuaRef include = it.value();
																if (!include.isNil()) {

																	luabridge::LuaRef includeName = include["name"];
																	if (!includeName.isNil()) {
																		componentInfo.name_ = includeName.cast<std::string>().value();
																	}
																	else {
																		componentInfo.name_ = include.cast<std::string>().value();
																	}
																	luabridge::LuaRef isIncludeReadonly = include["readonly"];
																	if (!isIncludeReadonly.isNil()) {
																		componentInfo.readonly_ = isIncludeReadonly.cast<bool>().value();
																	}

																}

																processesComponents.push_back(componentInfo);
															}
															CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(
																proccessEntityId,
																processesComponents);

															}(entity);

														//ASSERT_FMSG(
														//	(luaRef["processesComponents"].isNil() && !processesAllCombinations.isNil())
														//	|| (!luaRef["processesComponents"].isNil() && processesAllCombinations.isNil()),
														//	"System description error."
														//	" Using processesAllCombinations && processesComponents together is incorrect.");

														//Parse excludes.
														[&](luabridge::LuaRef luaRef) {

															luabridge::LuaRef entityExcludes = luaRef["excludes"];
															std::vector<std::string> parsedEntityExcludes;
															if (!entityExcludes.isNil()) {
																for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
																	luabridge::LuaRef exclude = it.value();
																	parsedEntityExcludes.push_back({ exclude.cast<std::string>().value() });

																	//ASSERT_FMSG(std::isupper(parsedEntityExcludes.back().name_[0]), "");

																}
															}
															CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ExcludeComponents>(
																proccessEntityId,
																parsedEntityExcludes);

															}(entity);

														//Parse removes.
														[&](luabridge::LuaRef systemOrEntityRef) {

															luabridge::LuaRef entityRemoves = systemOrEntityRef["removesComponents"];

															std::vector<std::string> parsedEntityRemoves;
															if (!entityRemoves.isNil()) {
																for (luabridge::Iterator it(entityRemoves); !it.isNil(); ++it) {
																	luabridge::LuaRef toRemove = it.value();
																	parsedEntityRemoves.push_back(toRemove.cast<std::string>().value());
																}
															}
															CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::RemoveComponents>(
																proccessEntityId,
																parsedEntityRemoves);

															}(entity);

														[&](luabridge::LuaRef luaRef) {

															luabridge::LuaRef entityExcludes = luaRef["accessingComponents"];

															std::vector<std::string> parsedEntityAccessing;
															if (!entityExcludes.isNil()) {
																for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
																	luabridge::LuaRef exclude = it.value();
																	parsedEntityAccessing.push_back(exclude.cast<std::string>().value());
																}
															}
															CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::AccessComponents>(
																proccessEntityId,
																parsedEntityAccessing);

															}(entity);

														proccessesEntities.push_back(proccessEntityId);

													}
													CreateComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(
														updateMethodEntityId, proccessesEntities);

													}(luaRef);

												//Parse Creates Entities.
												[&](luabridge::LuaRef luaRef) {

													std::vector<ECS2::Entity::Id> createsEntities_;

													luabridge::LuaRef createsEntity = luaRef["createsEntities"];
													if (!createsEntity.isNil()) {
														for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {

															ECS2::Entity::Id createEntityEntityId = CreateEntity<ECS__FILE__TABLE__SYSTEM__UPDATEMETHOD__CREATE__ENTITY__ENTITY>();

															//Parse Creates Components.
															[&](luabridge::LuaRef luaRef) {

																luabridge::LuaRef entityCreates = luaRef["createsComponents"];

																std::vector<std::string> parsedEntityCreates;
																if (!entityCreates.isNil()) {
																	for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
																		luabridge::LuaRef toCreate = it.value();
																		parsedEntityCreates.push_back(toCreate.cast<std::string>().value());
																	}
																}

																CreateComponent<ECS::File::Table::System::UpdateMethod::Create::Entity::CreateComponents>(
																	createEntityEntityId,
																	parsedEntityCreates);

																}(it.value());

															luabridge::LuaRef archetypeRef = it.value()["archetype"];
															if (!archetypeRef.isNil()) {

																CreateComponent<ECS::File::Table::System::UpdateMethod::Create::Entity::Archetype>(
																	createEntityEntityId,
																	ECS2::Entity::Id::invalid_,
																	archetypeRef.cast<std::string>().value()
																);
															}

															ASSERT_MSG(
																(luaRef["createsComponents"].isNil() && !archetypeRef.isNil()) ||
																(!luaRef["createsComponents"].isNil() && archetypeRef.isNil()),
																"Access entity can't creates archetype components and raw components at one time.");

															createsEntities_.push_back(createEntityEntityId);
														}
													}
													return createsEntities_;
													};
												CreateComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId, updateMethodEntityId);
												}(updateMethodRef);

										}

										return systemEntityId;
									}(table, name);
								return systemEntityId;
							}



							return ECS2::Entity::Id::invalid_;
					};

				for (auto it = luabridge::pairs(ecsFile).begin(); !it.isNil(); ++it) {

					luabridge::LuaRef key = it.key();
					luabridge::LuaRef value = it.value();

					//Key must be string.
					if (!key.isString()) {
						continue;
					}

					//Value must be a table(description of component, system...) or a string(component alias)
					if (!(value.isTable() || value.isString())) {
						continue;
					}

					const std::string keyString = key.tostring();
					luabridge::LuaRef table = key;



					const ECS2::Entity::Id parsedTableEntityId = processTable(keyString, ecsFile[key.tostring()]);
					
					if (parsedTableEntityId.IsValid()) {
						CreateComponent<File::EntityId>(parsedTableEntityId, entity0id);
						ECS2::Entity::Id namespaceTableEntityId = CreateEntity<ECS__FILE__TABLE__NAMESPACE__NAMESPACE>();
						CreateComponent<ECS::File::Table::Tag>(namespaceTableEntityId);
						CreateComponent<ECS::File::Table::Namespace::Tag>(namespaceTableEntityId);
						CreateComponent<ECS::File::Table::Name>(namespaceTableEntityId, "OksEngine");
						CreateComponent<ECS::File::Table::ChildTablesEntityIds>(
							namespaceTableEntityId,
							std::vector{ parsedTableEntityId });

						CreateComponent<ECS::File::Table::ParentTableEntityId>(parsedTableEntityId, namespaceTableEntityId);

						CreateComponent<File::EntityId>(namespaceTableEntityId, entity0id);
						parsedTables.push_back(namespaceTableEntityId);
					}
					
					//Separate namespace tables:
					//namespace {
					//	struct{}
					//	struct {}
					//}
					//to:
					//namespace {
					//struct {}
					// }
					// namespace {
					// struct {}
					// }
					//std::vector<ParsedTablePtr> separatedTables;

					//parsedTable->ForEachChildlessTable([&](ParsedTablePtr childlessTable) {

					//	ASSERT_FMSG(!childlessTable->HasChilds(), "");

					//	auto parentTables = childlessTable->GetParentTables();

					//	std::vector<std::shared_ptr<ParsedTable>> clonedTables;
					//	std::ranges::for_each(parentTables, [&](std::shared_ptr<ParsedTable> parentTable) {
					//		auto clonedParentTable = parentTable->Clone();
					//		clonedParentTable->childTables_.clear();
					//		clonedParentTable->parentTable_ = nullptr;
					//		clonedTables.push_back(clonedParentTable);

					//		});
					//	auto clonedChildlessTable = childlessTable->Clone();
					//	clonedChildlessTable->childTables_.clear();
					//	clonedChildlessTable->parentTable_ = nullptr;

					//	clonedTables.push_back(clonedChildlessTable);

					//	{
					//		const Common::Index lastTableIndex = clonedTables.size() - 1;
					//		for (Common::Index i = 0; i < clonedTables.size(); i++) {
					//			auto clonedTable = clonedTables[i];
					//			const Common::Index nextTableIndex = i + 1;
					//			if (nextTableIndex <= lastTableIndex) {
					//				auto nextClonedTable = clonedTables[nextTableIndex];
					//				nextClonedTable->parentTable_ = clonedTable;
					//				clonedTable->childTables_.push_back(nextClonedTable);
					//			}
					//		}
					//	}

					//	separatedTables.push_back(clonedTables[0]);

					//	return true;
					//	});

					////If there are no separated tables it means that parsed table doesnt have childs.
					//if (separatedTables.empty()) {
					//	separatedTables.push_back(parsedTable);
					//}

					//for (auto separatedTable : separatedTables) {
					//	parsedTables.push_back(separatedTable);
					//}
				}

			}
			CreateComponent<ECS::File::Table::EntityIds>(entity0id, parsedTables);
		}
	}
} // namespace OksEngine