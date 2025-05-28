#include <ECS/Scene\auto_OksEngine.StartWorldSceneSaving.hpp>

namespace OksEngine {
void StartWorldSceneSaving::Update(ECS2::Entity::Id entityId,
                                   const ECSController* eCSController,
                                   const SaveSceneRequest* saveSceneRequest) {

    std::string scene =
        "scene = {\n"
        "\tentities = {\n";

    CreateComponent<LuaScript>(entityId, scene);


};
}