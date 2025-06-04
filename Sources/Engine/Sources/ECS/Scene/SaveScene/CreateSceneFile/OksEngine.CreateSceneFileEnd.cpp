#include <ECS/Scene/SaveScene/CreateSceneFile\auto_OksEngine.CreateSceneFileEnd.hpp>

namespace OksEngine {
void CreateSceneFileEnd::Update(ECS2::Entity::Id entityId,
                                const ECSController* eCSController,
                                const SaveSceneRequest* saveSceneRequest,
                                LuaScript* luaScript) {

    luaScript->text_ += "}\n}\n";

};
}