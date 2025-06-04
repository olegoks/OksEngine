#include <Debug/ECS/ECSEditor/Resources\SaveECSFiles/auto_OksEngine.ProcessSaveECSFilesButton.hpp>

namespace OksEngine {
void ProcessSaveECSFilesButton::Update(ECS2::Entity::Id entityId,
                                      const ImGuiState* imGuiState,
                                      const EditorContext* editorContext) {

    if (ImGui::Button("Save changes")) {

        CreateComponent<SaveECSFilesButtonPressed>(entityId);
    };

};
}