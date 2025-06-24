#include <ECS/Scene/LoadScene\auto_OksEngine.CreateSceneEntities.hpp>


#include <OksEngine.ECS.hpp>

#include <auto_OksEngine.ParseEntity.hpp>

namespace OksEngine {
void CreateSceneEntities::Update(ECS2::Entity::Id entityId,
                                 const ECSController* eCSController,
                                 const LoadSceneRequest* loadSceneRequest,
                                 const LuaScript* luaScript) {
    

	::Lua::Context context;

	::Lua::Script script{ luaScript->text_ };
	context.LoadScript(script);

    luabridge::LuaRef scene = context.GetGlobalAsRef("scene");


    luabridge::LuaRef entities = scene["entities"];

	std::map<ECS2::Entity::Id, ECS2::Entity::Id> oldToNewId;
	std::vector<std::pair<luabridge::LuaRef, ECS2::Entity::Id>> entityLuaRefNewIds;

	for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
		ECS2::Entity::Id newEntityId = CreateEntity();
		luabridge::LuaRef entity = it.value();
		
		const ECS2::Entity::Id oldId = entity["ID"].cast<Common::Index>().value();
		oldToNewId[oldId] = newEntityId;
		entityLuaRefNewIds.push_back({ entity, newEntityId });
	}

	for (auto& [entityLuaRef, newEntityId] : entityLuaRefNewIds) {
		ParseEntity(world_, entityLuaRef, newEntityId, oldToNewId);
	}

	//Notify caller that scene loaded.

	if (IsEntityExist(loadSceneRequest->callerId_)) {
		CreateComponent<SceneLoaded>(loadSceneRequest->callerId_);
	}
	RemoveComponent<LoadSceneRequest>(entityId);
	RemoveComponent<LuaScript>(entityId);

};
}