#include <Debug/ECSCallGraphEditor/Resources\auto_OksEngine.ProcessSaveChangesButton.hpp>

namespace OksEngine {
void ProcessSaveChangesButton::Update(ECS2::Entity::Id entityId,
                                      const ImGuiState* imGuiState,
                                      const EditorContext* editorContext) {

    if (ImGui::Button("Save changes")) {

        CreateComponent<SaveChangesButtonPressed>(entityId);
    };

};
}