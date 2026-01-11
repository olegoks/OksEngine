#pragma once
#include <ECS\Scene\auto_OksEngine.Scene.hpp>

#include <auto_OksEngine.SerializeEntity.hpp>
#include <auto_OksEngine.ParseEntity.hpp>

#include <auto_OksEngine.Utils.hpp>

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


		::Lua::Context context;

		::Lua::Script script{ luaScript0->text_ };
		context.LoadScript(script);

		luabridge::LuaRef scene = context.GetGlobalAsRef("scene");


		luabridge::LuaRef entities = scene["entities"];

		std::map<ECS2::Entity::Id, ECS2::Entity::Id> oldToNewId;
		std::vector<std::pair<luabridge::LuaRef, ECS2::Entity::Id>> entityLuaRefNewIds;

		for (luabridge::Iterator it(entities); !it.isNil(); ++it) {

			luabridge::LuaRef entity = it.value();

			const ECS2::Entity::Id oldId = entity["ID"].cast<Common::Index>().value();

			ECS2::Entity::Id newEntityId = ECS2::Entity::Id::invalid_;
			if (!entity["ARCHETYPE"].isNil()) {
				const std::string archetypeName = entity["ARCHETYPE"].cast<std::string>().value();

				ECS2::ComponentsFilter archetypeComponentsFilter = GetArchetypeComponentsFilterByArchetypeName(archetypeName);

				newEntityId = world_->CreateEntity(archetypeComponentsFilter);

			}
			
			if (newEntityId.IsInvalid()) {
				newEntityId = CreateEntity();
			}


			oldToNewId[oldId] = newEntityId;
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