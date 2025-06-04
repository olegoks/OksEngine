#include <ECS/Scene\auto_OksEngine.StartWorldSceneSaving.hpp>

namespace OksEngine {
void StartWorldSceneSaving::Update(ECS2::Entity::Id entityId,
                                   const ECSController* eCSController,
                                   const SaveSceneRequest* saveSceneRequest) {
    CreateComponent<LuaScript>(entityId, "");

};
}