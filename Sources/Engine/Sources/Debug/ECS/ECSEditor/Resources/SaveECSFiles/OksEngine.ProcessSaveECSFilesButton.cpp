#include <Debug/ECS/ECSEditor/Resources\SaveECSFiles/auto_OksEngine.ProcessSaveECSFilesButton.hpp>

namespace OksEngine {
void ProcessSaveECSFilesButton::Update(ECS2::Entity::Id entityId,
                                      const ImGUI::State* imGuiState,
                                      const ECSEditorWindow* editorContext) {

    if (ImGui::Button("Save changes")) {

        CreateComponent<SaveECSFilesButtonPressed>(entityId);
    };

};
}