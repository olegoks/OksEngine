#pragma once
#include <ECS\Scene\auto_OksEngine.Scene.hpp>

#include <auto_OksEngine.SerializeEntity.hpp>
#include <auto_OksEngine.ParseEntity.hpp>

#include <auto_OksEngine.Utils.hpp>

#include <Resources/auto_OksEngine.ResourceSystem.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <regex>

namespace OksEngine
{
	namespace Scene::Manager {

		void CreateManager::Update() {

			const ECS2::Entity::Id  managerEntityId = CreateEntity();
			CreateComponent<Tag>(managerEntityId);
		}

		void ProcessLoadSceneRequest::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::Scene::Manager::Tag* scene__Manager__Tag0,
			ECS2::Entity::Id entity1id,
			const OksEngine::Scene::Manager::Request::Tag* scene__Manager__Request__Tag1,
			const OksEngine::Scene::Manager::Request::LoadScene* scene__Manager__Request__LoadScene1) {

			const ECS2::ComponentsFilter requestCF = GetComponentsFilter(entity1id);

			ASSERT_FMSG(requestCF.IsSetOnlyOneOf<SCENE__MANAGER__REQUEST__STATES>(),
				"Request must have only one of states.");

			if (requestCF.IsSet<Request::State::Pending>()) {
				const ECS2::Entity::Id loadResourceRequestId = RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(scene__Manager__Request__LoadScene1->sceneName_);
				CreateComponent<Resource::Manager::Request::EntityId>(entity1id, loadResourceRequestId);
				RemoveComponent<Request::State::Pending>(entity1id);
				CreateComponent<Request::State::InProgress>(entity1id);
			}
			else if (requestCF.IsSet<Request::State::InProgress>()) {
				ASSERT(requestCF.IsSet<Resource::Manager::Request::EntityId>());
				const ECS2::Entity::Id resourceRequestEntityId = GetComponent<Resource::Manager::Request::EntityId>(entity1id)->id_;
				const ECS2::ComponentsFilter resourceRequestCF = GetComponentsFilter(resourceRequestEntityId);
				if (resourceRequestCF.IsSet<Resource::Manager::Request::State::Finished>()) {
					const ECS2::Entity::Id resourceEntityId = GetComponent<Resource::EntityId>(resourceRequestEntityId)->id_;
					auto* resourceData = GetComponent<Resource::Data>(resourceEntityId);

					std::string sceneText{ resourceData->data_.data(), resourceData->data_.size() };

					std::regex setStringIdRegex("ID\\s*=\\s*(\"ID:[^\"]*\")");
					//ID = 10
					std::regex setNumberIdRegex(R"(ID\s*=\s*(\d+))");

					//Preprocess entity ids/names. Replace "ID:phys_plane"(example) with new entity id.
					std::string text = sceneText;

					auto findUsedSceneIds = [&setNumberIdRegex](const std::string& text) {
						std::unordered_set<ECS2::Entity::Id::ValueType> usedSceneIds;

						auto begin = std::sregex_iterator(text.begin(), text.end(), setNumberIdRegex);
						auto end = std::sregex_iterator();

						for (auto it = begin; it != end; ++it) {
							std::smatch match = *it;
							// match[1] содержит строку с числом
							int number = std::stoull(match[1].str());
							usedSceneIds.insert(number);
						}
						return usedSceneIds;
						};

					std::unordered_set<ECS2::Entity::Id::ValueType> usedSceneIds = findUsedSceneIds(text);

					auto getUniqueSceneId = [](std::unordered_set<ECS2::Entity::Id::ValueType>& usedSceneIds) -> ECS2::Entity::Id::ValueType {

						for (ECS2::Entity::Id::ValueType i = 0;
							i < Common::Limits<ECS2::Entity::Id::ValueType>::Max();
							i++) {
							if (!usedSceneIds.contains(i)) {
								usedSceneIds.insert(i);
								return i;
							}
						}

						ASSERT_FAIL();
						return ECS2::Entity::Id::invalid_.GetRawValue();
						};

					auto generateNumberIds = [&](const std::string& text) {

						auto begin = std::sregex_iterator(text.begin(), text.end(), setStringIdRegex);
						auto end = std::sregex_iterator();

						std::unordered_map<std::string, ECS2::Entity::Id::ValueType> matches;
						for (auto it = begin; it != end; ++it) {
							matches[(*it)[1].str()] = getUniqueSceneId(usedSceneIds);
						}

						return matches;
						};

					//"ID:example_string" -> 123
					std::unordered_map<std::string, ECS2::Entity::Id::ValueType> stringIdToNumberId = generateNumberIds(text);

					for (auto it = stringIdToNumberId.begin(); it != stringIdToNumberId.end(); ++it) {

						const std::string& stringId = it->first;
						const ECS2::Entity::Id::ValueType numberId = it->second;

						std::regex stringIdRegex(stringId);

						auto begin = std::sregex_iterator(text.begin(), text.end(), stringIdRegex);
						auto end = std::sregex_iterator();

						std::vector<std::pair<size_t, size_t>> positions; // {начало, длина}

						for (auto it = begin; it != end; ++it) {
							std::smatch match = *it;
							positions.push_back({ match.position(0), match.length() });
						}

						for (int i = positions.size() - 1; i >= 0; --i) {
							size_t pos = positions[i].first;
							size_t len = positions[i].second;
							std::string replacement = std::to_string(numberId); // число = порядковый номер
							text.replace(pos, len, replacement);
						}

					}

					::Lua::Context context;

					::Lua::Script script{ text/*luaScript0->text_*/ };
					context.LoadScript(script);

					luabridge::LuaRef scene = context.GetGlobalAsRef("scene");

					luabridge::LuaRef entities = scene["entities"];

					std::map<ECS2::Entity::Id, ECS2::Entity::Id> oldToNewId;
					std::map<std::string, ECS2::Entity::Id> entityNameToNewId;
					std::vector<std::pair<luabridge::LuaRef, ECS2::Entity::Id>> entityLuaRefNewIds;

					for (luabridge::Iterator it(entities); !it.isNil(); ++it) {

						luabridge::LuaRef entity = it.value();

						ECS2::Entity::Id newEntityId = ECS2::Entity::Id::invalid_;
						if (!entity["ARCHETYPE"].isNil()) {
							const std::string archetypeName = entity["ARCHETYPE"].cast<std::string>().value();

							ECS2::ComponentsFilter archetypeComponentsFilter = GetArchetypeComponentsFilterByArchetypeName(archetypeName);

							newEntityId = world_->CreateEntity(archetypeComponentsFilter);

						}

						if (newEntityId.IsInvalid()) {
							newEntityId = CreateEntity();
						}

						luabridge::LuaRef id = entity["ID"];

						//if (id.isNumber()) {
						const ECS2::Entity::Id oldId = entity["ID"].cast<ECS2::Entity::Id::ValueType>().value();
						oldToNewId[oldId] = newEntityId;
						//}
						/*else if (id.isString()) {
							const std::string entityName = entity["ID"].cast<std::string>().value();
							entityNameToNewId[entityName] = newEntityId
						}*/


						entityLuaRefNewIds.push_back({ entity, newEntityId });
					}

					for (auto& [entityLuaRef, newEntityId] : entityLuaRefNewIds) {

						for (luabridge::Iterator componentIt(entityLuaRef); !componentIt.isNil(); ++componentIt) {
							std::string fieldName = componentIt.key().tostring();

							ASSERT_FMSG(allowedSceneComponents.contains(fieldName), 
								"Scene: {}. Unknown scene entity component name {}. " 
								"Maybe you changed component name in .ecs and you must update this in .scn files."
								"Maybe you mean: {}",
								scene__Manager__Request__LoadScene1->sceneName_,
								fieldName, [&fieldName]() ->std::string {
								
									std::string target = fieldName;
									AI_GENERATED
									// Функция вычисления расстояния Левенштейна (итеративная версия)

									
									// Поиск строки с минимальным расстоянием
									auto it = std::min_element(allowedSceneComponents.begin(), allowedSceneComponents.end(),
										[target](const std::string& a, const std::string& b) {

											auto levenshteinDistance = [](const std::string& s1, const std::string& s2) ->size_t {
												const size_t m = s1.size();
												const size_t n = s2.size();

												// Если одна из строк пустая, расстояние равно длине другой
												if (m == 0) return n;
												if (n == 0) return m;

												std::vector<size_t> prev(n + 1);
												std::vector<size_t> curr(n + 1);

												for (size_t j = 0; j <= n; ++j)
													prev[j] = j;

												for (size_t i = 1; i <= m; ++i) {
													curr[0] = i;
													for (size_t j = 1; j <= n; ++j) {
														if (s1[i - 1] == s2[j - 1])
															curr[j] = prev[j - 1];
														else
															curr[j] = 1 + std::min({ prev[j], curr[j - 1], prev[j - 1] });
													}
													std::swap(prev, curr);
												}
												return prev[n];
												};


											return levenshteinDistance(a, target) < levenshteinDistance(b, target);
										});

									if (it != allowedSceneComponents.end()) {
										return *it;
										/*std::cout << "Most similar: " << *it
											<< " (distance = " << levenshteinDistance(*it, target) << ")\n";*/
									}

									return "<not_found>";
								
								}());

						}

						ParseEntity(world_, entityLuaRef, newEntityId, oldToNewId);
					}

					RemoveComponent<Scene::Manager::Request::State::InProgress>(entity1id);
					CreateComponent<Scene::Manager::Request::State::Finished>(entity1id);

				}


			}

		}

		void CreateLoadCommandLineSceneRequest::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::CommandLineParameters* commandLineParameters0,
			const OksEngine::SceneParameter* sceneParameter0) {

			static bool called = false;
			if (called) {
				return;
			}
			called = true;


			const ECS2::Entity::Id entityId = CreateEntity();
			CreateComponent<Scene::Manager::Request::Tag>(entityId);
			CreateComponent<Scene::Manager::Request::State::Pending>(entityId);
			CreateComponent<Scene::Manager::Request::LoadScene>(
				entityId,
				sceneParameter0->path_);

		}
	}
	//void StartWorldSceneSaving::Update(
	//	ECS2::Entity::Id entity0id,
	//	const ECSController* eCSController0,
	//	const SaveSceneRequest* saveSceneRequest0) {

	//	CreateComponent<LuaScript>(entity0id, "");

	//};

	//void CreateSceneFileStart::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
	//	const SaveSceneRequest* saveSceneRequest0, LuaScript* luaScript0) {

	//	std::string scene =
	//		"scene = {\n"
	//		"\tentities = {\n";

	//	luaScript0->text_ = scene;

	//};

	//void CreateSceneFile::Update(
	//	ECS2::Entity::Id entity0id,
	//	const Serializable* serializable0,

	//	ECS2::Entity::Id entity1id,
	//	const ECSController* eCSController1,
	//	const SaveSceneRequest* saveSceneRequest1,
	//	LuaScript* luaScript1) {


	//	const ECS2::ComponentsFilter components = GetComponentsFilter(entity0id);
	//	std::string parsedEntity;

	//	//If its not first entity add ",".
	//	{

	//		const Common::Index closingCurlyBraceIndex = luaScript1->text_.rfind('}');
	//		if (closingCurlyBraceIndex != std::string::npos) {
	//			//Not first entity.
	//			const Common::Index openingCurlyBraceIndex = luaScript1->text_.rfind('{');
	//			if (closingCurlyBraceIndex > openingCurlyBraceIndex) {
	//				luaScript1->text_ += ",";
	//			}
	//		}

	//	}

	//	luaScript1->text_ += parsedEntity + "\n\t{\n";

	//	std::string serializedEntity = SerializeEntity(world_, entity0id);

	//	//Remove last comma.
	//	serializedEntity.erase(serializedEntity.rfind(','));

	//	luaScript1->text_ += serializedEntity;

	//	luaScript1->text_ += parsedEntity + "\n\t}";

	//};


	//void CreateSceneFileEnd::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
	//	const SaveSceneRequest* saveSceneRequest0, LuaScript* luaScript0) {


	//	luaScript0->text_ += "}\n}\n";

	//};


	//void EndWorldSceneSaving::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
	//	const SaveSceneRequest* saveSceneRequest0, const LuaScript* luaScript0) {

	//	RemoveComponent<LuaScript>(entity0id);
	//	RemoveComponent<SaveSceneRequest>(entity0id);

	//};

	//void SaveSceneFile::Update(
	//	ECS2::Entity::Id entity0id, const CommandLineParameters* commandLineParameters0,
	//	const ConfigFilePath* configFilePath0, ECS2::Entity::Id entity1id, const ECSController* eCSController1,
	//	const SaveSceneRequest* saveSceneRequest1, const LuaScript* luaScript1,
	//	ECS2::Entity::Id entity2id, ResourceSystem* resourceSystem2, ECS2::Entity::Id entity3id,
	//	const Config* config3, const LuaScript* luaScript3) {

	//	::Lua::Context context;

	//	::Lua::Script script{ luaScript3->text_ };
	//	context.LoadScript(script);

	//	std::filesystem::path scenesRootDirectory =
	//		std::filesystem::path{ configFilePath0->path_ }.parent_path()
	//		/ context.GetGlobalAs<std::string>("scenesRootDirectory");

	//	Resources::ResourceData sceneFileData{
	//		luaScript1->text_.data(),
	//		luaScript1->text_.size()
	//	};

	//	//auto createSceneFileTask
	//	//	= resourceSystem2->system_->CreateNewFile(
	//	//		Subsystem::Type::ChildThread, scenesRootDirectory / ("OksEngine.Test.scn"),
	//	//		std::move(sceneFileData)); // draw graph settings
	//	//resourceSystem2->system_->CreateNewFile(Subsystem::Type::ChildThread, createSceneFileTask);

	//	const bool isSuccess = resourceSystem2->system_->CreateNewFileSynch(
	//		Subsystem::Type::Engine,
	//		scenesRootDirectory / ("OksEngine.Test.scn"),
	//		std::move(sceneFileData));

	//	OS::Assert(isSuccess);

	//};



	//void FindScenes::Update(
	//	ECS2::Entity::Id entity0id,
	//	ResourceSystem* resourceSystem0) {



	//	auto taskId = resourceSystem0->system_->GetAddedResources(Subsystem::Type::Engine, { ".scn" });
	//	auto addedResource = resourceSystem0->system_->GetAddedResources(Subsystem::Type::Engine, taskId);

	//	std::vector<AsyncResourceSubsystem::Task::Id> getResourceTaskIds;

	//	for (auto& ecsFilePath : addedResource) {
	//		getResourceTaskIds.push_back(
	//			resourceSystem0->system_->GetResource(
	//				Subsystem::Type::Engine,
	//				ecsFilePath));
	//	}

	//	std::vector<Resources::ResourceData> scnResources;

	//	for (auto& taskId : getResourceTaskIds) {
	//		scnResources.push_back(resourceSystem0->system_->GetResource(Subsystem::Type::Engine, taskId));
	//	}


	//};


	//void LoadSceneFile::Update(

	//	ECS2::Entity::Id entity0id,
	//	const ECSController* eCSController0,
	//	const LoadSceneRequest* loadSceneRequest0,

	//	ECS2::Entity::Id entity1id,
	//	ResourceSystem* resourceSystem1) {

	//	auto sceneFileData =
	//		resourceSystem1->system_->GetResourceSynch(
	//			Subsystem::Type::Engine,
	//			"Root/" + loadSceneRequest0->sceneName_);

	//	CreateComponent<LuaScript>(entity0id,
	//		std::string{
	//			sceneFileData.GetData<Common::Byte>(),
	//			sceneFileData.GetSize()
	//		});

	//}

	//void CreateSceneEntities::Update(
	//	ECS2::Entity::Id entity0id,
	//	const ECSController* eCSController0,
	//	const LoadSceneRequest* loadSceneRequest0,
	//	const LuaScript* luaScript0) {

	//	//ID = "ID:example_name" regex
	//	std::regex setStringIdRegex("ID\\s*=\\s*(\"ID:[^\"]*\")");
	//	//ID = 10
	//	std::regex setNumberIdRegex(R"(ID\s*=\s*(\d+))");

	//	//Preprocess entity ids/names. Replace "ID:phys_plane"(example) with new entity id.
	//	std::string text = luaScript0->text_;

	//	auto findUsedSceneIds = [&setNumberIdRegex](const std::string& text) {
	//		std::unordered_set<ECS2::Entity::Id::ValueType> usedSceneIds;

	//		auto begin = std::sregex_iterator(text.begin(), text.end(), setNumberIdRegex);
	//		auto end = std::sregex_iterator();

	//		for (auto it = begin; it != end; ++it) {
	//			std::smatch match = *it;
	//			// match[1] содержит строку с числом
	//			int number = std::stoull(match[1].str());
	//			usedSceneIds.insert(number);
	//		}
	//		return usedSceneIds;
	//		};

	//	std::unordered_set<ECS2::Entity::Id::ValueType> usedSceneIds = findUsedSceneIds(text);

	//	auto getUniqueSceneId = [](std::unordered_set<ECS2::Entity::Id::ValueType>& usedSceneIds) -> ECS2::Entity::Id::ValueType {

	//		for (ECS2::Entity::Id::ValueType i = 0;
	//			i < Common::Limits<ECS2::Entity::Id::ValueType>::Max();
	//			i++) {
	//			if (!usedSceneIds.contains(i)) {
	//				usedSceneIds.insert(i);
	//				return i;
	//			}
	//		}

	//		ASSERT_FAIL();
	//		return ECS2::Entity::Id::invalid_.GetRawValue();
	//		};

	//	auto generateNumberIds = [&](const std::string& text) {

	//		auto begin = std::sregex_iterator(text.begin(), text.end(), setStringIdRegex);
	//		auto end = std::sregex_iterator();

	//		std::unordered_map<std::string, ECS2::Entity::Id::ValueType> matches;
	//		for (auto it = begin; it != end; ++it) {
	//			matches[(*it)[1].str()] = getUniqueSceneId(usedSceneIds);
	//		}

	//		return matches;
	//		};

	//	//"ID:example_string" -> 123
	//	std::unordered_map<std::string, ECS2::Entity::Id::ValueType> stringIdToNumberId = generateNumberIds(text);

	//	for (auto it = stringIdToNumberId.begin(); it != stringIdToNumberId.end(); ++it) {

	//		const std::string& stringId = it->first;
	//		const ECS2::Entity::Id::ValueType numberId = it->second;

	//		std::regex stringIdRegex(stringId);

	//		auto begin = std::sregex_iterator(text.begin(), text.end(), stringIdRegex);
	//		auto end = std::sregex_iterator();

	//		std::vector<std::pair<size_t, size_t>> positions; // {начало, длина}

	//		for (auto it = begin; it != end; ++it) {
	//			std::smatch match = *it;
	//			positions.push_back({ match.position(0), match.length() });
	//		}

	//		for (int i = positions.size() - 1; i >= 0; --i) {
	//			size_t pos = positions[i].first;
	//			size_t len = positions[i].second;
	//			std::string replacement = std::to_string(numberId); // число = порядковый номер
	//			text.replace(pos, len, replacement);
	//		}

	//	}

	//	::Lua::Context context;

	//	::Lua::Script script{ text/*luaScript0->text_*/ };
	//	context.LoadScript(script);

	//	luabridge::LuaRef scene = context.GetGlobalAsRef("scene");

	//	luabridge::LuaRef entities = scene["entities"];

	//	std::map<ECS2::Entity::Id, ECS2::Entity::Id> oldToNewId;
	//	std::map<std::string, ECS2::Entity::Id> entityNameToNewId;
	//	std::vector<std::pair<luabridge::LuaRef, ECS2::Entity::Id>> entityLuaRefNewIds;

	//	for (luabridge::Iterator it(entities); !it.isNil(); ++it) {

	//		luabridge::LuaRef entity = it.value();

	//		ECS2::Entity::Id newEntityId = ECS2::Entity::Id::invalid_;
	//		if (!entity["ARCHETYPE"].isNil()) {
	//			const std::string archetypeName = entity["ARCHETYPE"].cast<std::string>().value();

	//			ECS2::ComponentsFilter archetypeComponentsFilter = GetArchetypeComponentsFilterByArchetypeName(archetypeName);

	//			newEntityId = world_->CreateEntity(archetypeComponentsFilter);

	//		}

	//		if (newEntityId.IsInvalid()) {
	//			newEntityId = CreateEntity();
	//		}

	//		luabridge::LuaRef id = entity["ID"];

	//		//if (id.isNumber()) {
	//		const ECS2::Entity::Id oldId = entity["ID"].cast<ECS2::Entity::Id::ValueType>().value();
	//		oldToNewId[oldId] = newEntityId;
	//		//}
	//		/*else if (id.isString()) {
	//			const std::string entityName = entity["ID"].cast<std::string>().value();
	//			entityNameToNewId[entityName] = newEntityId
	//		}*/


	//		entityLuaRefNewIds.push_back({ entity, newEntityId });
	//	}

	//	for (auto& [entityLuaRef, newEntityId] : entityLuaRefNewIds) {
	//		ParseEntity(world_, entityLuaRef, newEntityId, oldToNewId);
	//	}

	//	//Notify caller that scene loaded.
	//	if (IsEntityExist(loadSceneRequest0->callerId_)) {
	//		CreateComponent<SceneLoaded>(loadSceneRequest0->callerId_);
	//	}
	//	RemoveComponent<LoadSceneRequest>(entity0id);
	//	RemoveComponent<LuaScript>(entity0id);

	//}


	//void CreateLoadSceneRequest::Update(
	//	ECS2::Entity::Id entity0id,
	//	const CommandLineParameters* commandLineParameters0,
	//	const SceneParameter* sceneParameter0,

	//	ECS2::Entity::Id entity1id,
	//	const ECSController* eCSController1) {

	//	const std::filesystem::path scenePath = sceneParameter0->path_;


	//	CreateComponent<LoadSceneRequest>(entity1id, scenePath.filename().string(), entity0id);

	//}


} // namespace OksEngine