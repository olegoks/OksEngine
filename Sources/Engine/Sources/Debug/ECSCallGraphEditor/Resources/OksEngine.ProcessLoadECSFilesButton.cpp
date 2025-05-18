#include <Debug/ECSCallGraphEditor\Resources/auto_OksEngine.ProcessLoadECSFilesButton.hpp>

namespace OksEngine {
    void ProcessLoadECSFilesButton::Update(
        ECS2::Entity::Id entityId, 
        const ImGuiState* imGuiState,
        const EditorContext* editorContext) {

        if (ImGui::Button("Load .ecs files")) {

            CreateComponent<LoadECSFilesButtonPressed>(entityId);
        };
}
}