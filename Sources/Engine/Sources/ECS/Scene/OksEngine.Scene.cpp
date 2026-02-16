#pragma once
#include <ECS\Scene\auto_OksEngine.Scene.hpp>

#include <auto_OksEngine.SerializeEntity.hpp>
#include <auto_OksEngine.ParseEntity.hpp>

#include <auto_OksEngine.Utils.hpp>

#include <regex>

namespace OksEngine
{

	void StartWorldSceneSaving::Update(
		ECS2::Entity::Id entity0id,
		const ECSController* eCSController0,
		const SaveSceneRequest* saveSceneRequest0) {

		CreateComponent<LuaScript>(entity0id, "");

	};

	void CreateSceneFileStart::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
		const SaveSceneRequest* saveSceneRequest0, LuaScript* luaScript0) {

		std::string scene =
			"scene = {\n"
			"\tentities = {\n";

		luaScript0->text_ = scene;

	};

	void CreateSceneFile::Update(
		ECS2::Entity::Id entity0id,
		const Serializable* serializable0,

		ECS2::Entity::Id entity1id,
		const ECSController* eCSController1,
		const SaveSceneRequest* saveSceneRequest1,
		LuaScript* luaScript1) {


		const ECS2::ComponentsFilter components = GetComponentsFilter(entity0id);
		std::string parsedEntity;

		//If its not first entity add ",".
		{

			const Common::Index closingCurlyBraceIndex = luaScript1->text_.rfind('}');
			if (closingCurlyBraceIndex != std::string::npos) {
				//Not first entity.
				const Common::Index openingCurlyBraceIndex = luaScript1->text_.rfind('{');
				if (closingCurlyBraceIndex > openingCurlyBraceIndex) {
					luaScript1->text_ += ",";
				}
			}

		}

		luaScript1->text_ += parsedEntity + "\n\t{\n";

		std::string serializedEntity = SerializeEntity(world_, entity0id);

		//Remove last comma.
		serializedEntity.erase(serializedEntity.rfind(','));

		luaScript1->text_ += serializedEntity;

		luaScript1->text_ += parsedEntity + "\n\t}";

	};


	void CreateSceneFileEnd::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
		const SaveSceneRequest* saveSceneRequest0, LuaScript* luaScript0) {


		luaScript0->text_ += "}\n}\n";

	};


	void EndWorldSceneSaving::Update(ECS2::Entity::Id entity0id, const ECSController* eCSController0,
		const SaveSceneRequest* saveSceneRequest0, const LuaScript* luaScript0) {

		RemoveComponent<LuaScript>(entity0id);
		RemoveComponent<SaveSceneRequest>(entity0id);

	};

	void SaveSceneFile::Update(
		ECS2::Entity::Id entity0id, const CommandLineParameters* commandLineParameters0,
		const ConfigFilePath* configFilePath0, ECS2::Entity::Id entity1id, const ECSController* eCSController1,
		const SaveSceneRequest* saveSceneRequest1, const LuaScript* luaScript1,
		ECS2::Entity::Id entity2id, ResourceSystem* resourceSystem2, ECS2::Entity::Id entity3id,
		const Config* config3, const LuaScript* luaScript3) {

		::Lua::Context context;

		::Lua::Script script{ luaScript3->text_ };
		context.LoadScript(script);

		std::filesystem::path scenesRootDirectory =
			std::filesystem::path{ configFilePath0->path_ }.parent_path()
			/ context.GetGlobalAs<std::string>("scenesRootDirectory");

		Resources::ResourceData sceneFileData{
			luaScript1->text_.data(),
			luaScript1->text_.size()
		};

		//auto createSceneFileTask
		//	= resourceSystem2->system_->CreateNewFile(
		//		Subsystem::Type::ChildThread, scenesRootDirectory / ("OksEngine.Test.scn"),
		//		std::move(sceneFileData)); // draw graph settings
		//resourceSystem2->system_->CreateNewFile(Subsystem::Type::ChildThread, createSceneFileTask);

		const bool isSuccess = resourceSystem2->system_->CreateNewFileSynch(
			Subsystem::Type::Engine,
			scenesRootDirectory / ("OksEngine.Test.scn"),
			std::move(sceneFileData));

		OS::Assert(isSuccess);

	};



	void FindScenes::Update(
		ECS2::Entity::Id entity0id,
		ResourceSystem* resourceSystem0) {



		auto taskId = resourceSystem0->system_->GetAddedResources(Subsystem::Type::Engine, { ".scn" });
		auto addedResource = resourceSystem0->system_->GetAddedResources(Subsystem::Type::Engine, taskId);

		std::vector<AsyncResourceSubsystem::Task::Id> getResourceTaskIds;

		for (auto& ecsFilePath : addedResource) {
			getResourceTaskIds.push_back(
				resourceSystem0->system_->GetResource(
					Subsystem::Type::Engine,
					ecsFilePath));
		}

		std::vector<Resources::ResourceData> scnResources;

		for (auto& taskId : getResourceTaskIds) {
			scnResources.push_back(resourceSystem0->system_->GetResource(Subsystem::Type::Engine, taskId));
		}


	};


	void LoadSceneFile::Update(

		ECS2::Entity::Id entity0id,
		const ECSController* eCSController0,
		const LoadSceneRequest* loadSceneRequest0,

		ECS2::Entity::Id entity1id,
		ResourceSystem* resourceSystem1) {


		auto sceneFileData =
			resourceSystem1->system_->GetResourceSynch(
				Subsystem::Type::Engine,
				"Root/" + loadSceneRequest0->sceneName_);

		CreateComponent<LuaScript>(entity0id,
			std::string{
				sceneFileData.GetData<Common::Byte>(),
				sceneFileData.GetSize()
			});

	}

	void CreateSceneEntities::Update(
		ECS2::Entity::Id entity0id,
		const ECSController* eCSController0,
		const LoadSceneRequest* loadSceneRequest0,
		const LuaScript* luaScript0) {

		//ID = "ID:example_name" regex
		std::regex setStringIdRegex("ID\\s*=\\s*(\"ID:[^\"]*\")");
		//ID = 10
		std::regex setNumberIdRegex(R"(ID\s*=\s*(\d+))");

		//Preprocess entity ids/names. Replace "ID:phys_plane"(example) with new entity id.
		std::string text = luaScript0->text_;

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
			ParseEntity(world_, entityLuaRef, newEntityId, oldToNewId);
		}

		//Notify caller that scene loaded.
		if (IsEntityExist(loadSceneRequest0->callerId_)) {
			CreateComponent<SceneLoaded>(loadSceneRequest0->callerId_);
		}
		RemoveComponent<LoadSceneRequest>(entity0id);
		RemoveComponent<LuaScript>(entity0id);

	}


	void CreateLoadSceneRequest::Update(
		ECS2::Entity::Id entity0id,
		const CommandLineParameters* commandLineParameters0,
		const SceneParameter* sceneParameter0,

		ECS2::Entity::Id entity1id,
		const ECSController* eCSController1) {

		const std::filesystem::path scenePath = sceneParameter0->path_;


		CreateComponent<LoadSceneRequest>(entity1id, scenePath.filename().string(), entity0id);

	}


} // namespace OksEngine