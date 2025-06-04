#include <ECS/Scene\auto_OksEngine.EndWorldSceneSaving.hpp>

namespace OksEngine {
void EndWorldSceneSaving::Update(
    ECS2::Entity::Id entityId, const ECSController* eCSController,
    const SaveSceneRequest* saveSceneRequest,
    const LuaScript* luaScript) {

    RemoveComponent<LuaScript>(entityId);
    RemoveComponent<SaveSceneRequest>(entityId);


};
}