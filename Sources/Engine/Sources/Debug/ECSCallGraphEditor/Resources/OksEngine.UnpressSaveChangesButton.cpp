#include <Debug/ECSCallGraphEditor/Resources\auto_OksEngine.UnpressSaveChangesButton.hpp>

namespace OksEngine {
void UnpressSaveChangesButton::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const EditorContext* editorContext,
    const SaveChangesButtonPressed* saveChangesButtonPressed) {

		RemoveComponent<SaveChangesButtonPressed>(entityId);

    };
}