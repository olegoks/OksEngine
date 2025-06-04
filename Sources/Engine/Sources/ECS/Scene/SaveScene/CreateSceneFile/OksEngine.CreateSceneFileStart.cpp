#include <ECS/Scene/SaveScene/CreateSceneFile\auto_OksEngine.CreateSceneFileStart.hpp>

namespace OksEngine {
void CreateSceneFileStart::Update(
    ECS2::Entity::Id entity0id, 
    const ECSController* eCSController0,
    const SaveSceneRequest* saveSceneRequest0,
    LuaScript* luaScript0) {

    std::string scene =
        "scene = {\n"
        "\tentities = {\n";

    luaScript0->text_ = scene;


};
}