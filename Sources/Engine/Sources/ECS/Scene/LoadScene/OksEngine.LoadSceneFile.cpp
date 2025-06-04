#include <ECS/Scene/LoadScene\auto_OksEngine.LoadSceneFile.hpp>

namespace OksEngine {
void LoadSceneFile::Update(

    ECS2::Entity::Id entity0id,
    const ECSController* eCSController0,
    const LoadSceneRequest* loadSceneRequest0,

    ECS2::Entity::Id entity1id, 
    ResourceSystem* resourceSystem1) {
    
    auto sceneFileData = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/" + loadSceneRequest0->sceneName_);

    CreateComponent<LuaScript>(entity0id,
        std::string{ 
            sceneFileData.GetData<Common::Byte>(),
            sceneFileData.GetSize() 
        });

};
}