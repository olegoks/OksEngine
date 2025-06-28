#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles\auto_OksEngine.EndLoadingECSFiles.hpp>

namespace OksEngine {
void EndLoadingECSFiles::Update(
    ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
    const ECSEditorWindow* eCSEditorWindow0,
    const EditorContext* editorContext0,
    const CreateLinksRequest* createLinksRequest0) {


RemoveComponent<CreateLinksRequest>(entity0id);


};
}  // namespace OksEngine