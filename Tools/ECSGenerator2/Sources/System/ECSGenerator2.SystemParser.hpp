#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>

namespace ECSGenerator2 {

	class SystemParser {
	public:

		[[nodiscard]]
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef system,
			const std::string& systemName) {

			if (system["disable"].cast<bool>().value()) {
				return nullptr;
			}

			auto parseAfterUpdateMethod = [](luabridge::LuaRef luaRef) -> std::shared_ptr<ParsedSystem::AfterUpdateMethodInfo> {

				luabridge::LuaRef afterUpdateMethodRef = luaRef["afterUpdateMethod"];
				if (!afterUpdateMethodRef.isNil()) {
					return std::make_shared<ParsedSystem::AfterUpdateMethodInfo>();
				}
				return nullptr;

				};
			auto parseCallOrder = [](luabridge::LuaRef luaRef) -> std::shared_ptr<ParsedSystem::CallOrderInfo> {

				auto parseRunAfter = [](luabridge::LuaRef systemRef) {

					std::vector<ParsedSystem::CallOrderInfo::System> runAfterSystems;
					luabridge::LuaRef runAfterRef = systemRef["runAfter"];
					if (!runAfterRef.isNil()) {
						if (runAfterRef.isTable()) {
							for (luabridge::Iterator it(runAfterRef); !it.isNil(); ++it) {
								luabridge::LuaRef runAfterSystemRef = it.value();
								runAfterSystems.push_back({ runAfterSystemRef.cast<std::string>().value(), nullptr });
							}
						}
						else {
							runAfterSystems.push_back( { runAfterRef.cast<std::string>().value(), nullptr });
						}
					}
					return runAfterSystems;
					};
				auto parseRunBefore = [](luabridge::LuaRef systemRef) {
					std::vector<ParsedSystem::CallOrderInfo::System> runBeforeSystems;
					luabridge::LuaRef runBeforeRef = systemRef["runBefore"];
					if (!runBeforeRef.isNil()) {
						if (runBeforeRef.isTable()) {
							for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
								luabridge::LuaRef runAfterSystemRef = it.value();
								runBeforeSystems.push_back({ runAfterSystemRef.cast<std::string>().value(), nullptr });
							}
						}
						else {
							runBeforeSystems.push_back({ runBeforeRef.cast<std::string>().value(), nullptr });
						}
					}
					return runBeforeSystems;
					};

				luabridge::LuaRef callOrderRef = luaRef["callOrder"];

				if (!callOrderRef.isNil()) {
					const auto runAfterSystems = parseRunAfter(callOrderRef);
					const auto runBeforeSystems = parseRunBefore(callOrderRef);
					const auto callOrderInfo = std::make_shared<ParsedSystem::CallOrderInfo>(
						runAfterSystems,
						runBeforeSystems
					);
					return callOrderInfo;
				}

				return nullptr;

				};
			
			auto parseUpdateMethod = [&](luabridge::LuaRef luaRef) {

				auto parseAccessesEntities = [](luabridge::LuaRef systemRef) {

					std::vector<ParsedSystem::RandomAccessEntity> accessesEntities;

					//Access entities by id.
					luabridge::LuaRef accessingEntities = systemRef["accessingEntities"];


					if (!accessingEntities.isNil()) {
						for (luabridge::Iterator it(accessingEntities); !it.isNil(); ++it) {

							std::vector<ParsedSystem::Include> accessesComponents;
							std::vector<ParsedSystem::Create> createsComponents;

							luabridge::LuaRef toAccess = it.value();
							luabridge::LuaRef accessingComponentsRef = toAccess["accessingComponents"];
							for (luabridge::Iterator itJ(accessingComponentsRef); !itJ.isNil(); ++itJ) {
								luabridge::LuaRef accessingComponentRef = itJ.value();

								ParsedSystem::Include include{
									.name_ = accessingComponentRef.cast<std::string>().value(),
									.readonly_ = false // TODO: add option to access component only to read.
								};
								accessesComponents.push_back(include);
								OS::AssertMessage(std::isupper(accessesComponents.back().name_[0]), "");
							}
							luabridge::LuaRef createsComponentsRef = toAccess["createsComponents"];
							if (!createsComponentsRef.isNil()) {
								for (luabridge::Iterator itJ(createsComponentsRef); !itJ.isNil(); ++itJ) {
									luabridge::LuaRef createsComponentRef = itJ.value();
									createsComponents.push_back({ nullptr, createsComponentRef.cast<std::string>().value() });
									OS::AssertMessage(std::isupper(createsComponents.back().GetName()[0]), "");
								}
							}
							ParsedSystem::RandomAccessEntity randomAccessEntity{
								.includes_ = accessesComponents,
								.creates_ = createsComponents,
							};
							accessesEntities.push_back(randomAccessEntity);
						}
					}

					return accessesEntities;
					};
				auto parseProcessesEntities = [&](luabridge::LuaRef luaRef) {

					auto parseProcessesComponents = [](luabridge::LuaRef luaRef) {

						luabridge::LuaRef entityIncludes = luaRef["processesComponents"];
						std::vector<ParsedSystem::Include> parsedEntityIncludes;

						if (entityIncludes.isNil()) {
							return parsedEntityIncludes;
						}

						for (luabridge::Iterator it(entityIncludes); !it.isNil(); ++it) {

							luabridge::LuaRef include = it.value();
							ParsedSystem::Include parsedInclude;

							if (!include.isNil()) {
								luabridge::LuaRef includeName = include["name"];
								if (!includeName.isNil()) {
									parsedInclude.name_ = includeName.cast<std::string>().value();
								}
								else {
									parsedInclude.name_ = include.cast<std::string>().value();
								}
								luabridge::LuaRef isIncludeReadonly = include["readonly"];
								if (!isIncludeReadonly.isNil()) {
									parsedInclude.readonly_ = isIncludeReadonly.cast<bool>().value();
								}

							}

							parsedEntityIncludes.push_back(parsedInclude);
							OS::AssertMessage(std::isupper(parsedEntityIncludes.back().name_[0]), "Name of component must be uppercase");
						}

						return parsedEntityIncludes;
						};
					auto parseRemovesComponents = [](luabridge::LuaRef systemOrEntityRef) {

						luabridge::LuaRef entityRemoves = systemOrEntityRef["removesComponents"];

						std::vector<std::string> parsedEntityRemoves;
						if (!entityRemoves.isNil()) {
							for (luabridge::Iterator it(entityRemoves); !it.isNil(); ++it) {
								luabridge::LuaRef toRemove = it.value();
								parsedEntityRemoves.push_back(toRemove.cast<std::string>().value());
								OS::AssertMessage(std::isupper(parsedEntityRemoves.back()[0]), "");
							}
						}

						return parsedEntityRemoves;

						};
					auto parseCreatesComponents = [](luabridge::LuaRef systemOrEntityRef) {

						luabridge::LuaRef entityCreates = systemOrEntityRef["createsComponents"];

						std::vector<ParsedSystem::Create> parsedEntityCreates;
						if (!entityCreates.isNil()) {
							for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
								luabridge::LuaRef toCreate = it.value();
								parsedEntityCreates.push_back({ nullptr, toCreate.cast<std::string>().value() });
								OS::AssertMessage(std::isupper(parsedEntityCreates.back().GetName()[0]), "");
							}
						}

						return parsedEntityCreates;

						};
					auto parseExcludes = [](luabridge::LuaRef luaRef) {

						luabridge::LuaRef entityExcludes = luaRef["excludes"];
						std::vector<ParsedSystem::Exclude> parsedEntityExcludes;
						if (!entityExcludes.isNil()) {
							for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
								luabridge::LuaRef exclude = it.value();
								parsedEntityExcludes.push_back({ nullptr, exclude.cast<std::string>().value() });

								OS::AssertMessage(std::isupper(parsedEntityExcludes.back().name_[0]), "");

							}
						}
						return parsedEntityExcludes;

						};

					std::vector<ParsedSystem::ProcessedEntity> parsedEntities;

					luabridge::LuaRef entities = luaRef["processesEntities"];
					if (entities.isNil()) {
						return parsedEntities;
					}
					for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
						luabridge::LuaRef entity = it.value();

						luabridge::LuaRef processesAllCombinations = entity["processesAllCombinations"];

						std::vector<ParsedSystem::Include> parsedEntityIncludes
							= parseProcessesComponents(entity);

						OS::AssertMessage(
							(parsedEntityIncludes.empty() && !processesAllCombinations.isNil())
							|| (!parsedEntityIncludes.empty() && processesAllCombinations.isNil()),
							"System description error."
							" Using processesAllCombinations && processesComponents together is incorrect.");

						std::vector<ParsedSystem::Exclude> parsedEntityExcludes
							= parseExcludes(entity);

						std::vector<ParsedSystem::Create> createsComponents = parseCreatesComponents(entity);

						ParsedSystem::ProcessedEntity parsedEntity{
							.includes_ = parsedEntityIncludes,
							.processesAllCombinations_ = !processesAllCombinations.isNil(),
							.excludes_ = parsedEntityExcludes,
							.creates_ = createsComponents
						};
						parsedEntities.push_back(parsedEntity);

					}
					return parsedEntities;

					};
				auto parseRandomAccessComponents = [](luabridge::LuaRef systemOrEntityRef) {
					luabridge::LuaRef randomAccessComponentsRef = systemOrEntityRef["randomAccessComponents"];
					bool randomAccessComponents = false;
					if (!randomAccessComponentsRef.isNil()) {
						randomAccessComponents = randomAccessComponentsRef.cast<bool>().value();
					}
					//return randomAccessComponents;
					OS::AssertFail();
					return std::vector<ParsedSystem::RandomAccessEntity>{};
					};
				auto parseCreatesEntities = [&](luabridge::LuaRef luaRef) {

					auto parseCreatesComponents = [](luabridge::LuaRef luaRef) {

						luabridge::LuaRef entityCreates = luaRef["createsComponents"];

						std::vector<ParsedSystem::Create> parsedEntityCreates;
						if (!entityCreates.isNil()) {
							for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
								luabridge::LuaRef toCreate = it.value();
								parsedEntityCreates.push_back({ nullptr, toCreate.cast<std::string>().value() });
								OS::AssertMessage(std::isupper(parsedEntityCreates.back().GetName()[0]), "");
							}
						}

						return parsedEntityCreates;

						};

					std::vector<ParsedSystem::CreatesEntity> createsEntities_;

					luabridge::LuaRef createsEntity = luaRef["createsEntities"];
					if (!createsEntity.isNil()) {
						for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {

							std::vector<ParsedSystem::Create> createsComponents = parseCreatesComponents(it.value());

							createsEntities_.push_back(ParsedSystem::CreatesEntity{ createsComponents });
						}
					}
					return createsEntities_;
					};

				luabridge::LuaRef updateMethodRef = luaRef["updateMethod"];
				if (!updateMethodRef.isNil()) {
					auto processesEntities = parseProcessesEntities(updateMethodRef);
					auto randomAccessEntities = parseAccessesEntities(updateMethodRef);
					auto createsEntities = parseCreatesEntities(updateMethodRef);
					auto updateMethodInfo = std::make_shared<ParsedSystem::UpdateMethodInfo>(
						processesEntities,
						randomAccessEntities,
						createsEntities
					);
					return updateMethodInfo;
				}

				return std::shared_ptr<ParsedSystem::UpdateMethodInfo>();
				};
			
			auto parseThread = [](luabridge::LuaRef systemRef) {
				luabridge::LuaRef threadRef = systemRef["thread"];
				if (!threadRef.isNil()) {
					const std::string thread = threadRef.cast<std::string>().value();
					return ParsedSystem::StringToThread(thread);
				}
				//Default value.
				return ParsedSystem::Thread::Child;
				};
			auto parseType = [](luabridge::LuaRef systemRef) {
				luabridge::LuaRef typeRef = systemRef["type"];
				if (!typeRef.isNil()) {
					const std::string type = typeRef.cast<std::string>().value();
					return ParsedSystem::StringToType(type);
				}
				//Default value.
				return ParsedSystem::Type::FrameToFrame;

				};


			if (systemName == "AddModelToRender") {
				Common::BreakPointLine();
			}

			const ParsedSystem::Thread thread = parseThread(system);
			const ParsedSystem::Type type = parseType(system);

			auto afterUpdateMethodInfo = parseAfterUpdateMethod(system);
			auto callOrderInfo = parseCallOrder(system);


			//Parse update method.
			std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo = nullptr;
			if (!system["updateMethod"].isNil()) {
				updateMethodInfo = parseUpdateMethod(system);
			}

			//auto accessesEntities = parseAccessesEntities(system);

#pragma region Assert
			OS::AssertMessage(
				((updateMethodInfo == nullptr) && (type == ParsedSystem::Type::Initialize)) ||
				((updateMethodInfo != nullptr) && ((type == ParsedSystem::Type::FrameToFrame) || (type == ParsedSystem::Type::Initialize))),
				"System description doesn't contain updateMethod decription.");
#pragma endregion

			ParsedSystem::CreateInfo ci{
				.isEnabled_ = true,
				.name_ = systemName,
				.thread_ = thread,
				.type_ = type,
				.callOrderInfo_ = callOrderInfo,
				.updateMethod_ = updateMethodInfo,
				.afterUpdateMethod_ = afterUpdateMethodInfo
			};

			auto parsedSystemFile = std::make_shared<ParsedSystem>(ci);
			return parsedSystemFile;
		}

	};



}