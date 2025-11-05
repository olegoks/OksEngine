#include <Debug/ECS/ECSEditor\auto_OksEngine.RemoveSceneLoaded.hpp>

namespace OksEngine {
void RemoveSceneLoaded::Update(
    ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
    const EditorContext* editorContext0,
    const SceneLoaded* sceneLoaded0) {
    
    RemoveComponent<SceneLoaded>(entity0id);

};
}