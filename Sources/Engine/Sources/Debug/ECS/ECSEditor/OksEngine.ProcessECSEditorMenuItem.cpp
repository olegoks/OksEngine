#include <Debug/ECS/ECSEditor\auto_OksEngine.ProcessECSEditorMenuItem.hpp>

namespace OksEngine {
	void ProcessECSEditorMenuItem::Update(
		ECS2::Entity::Id entity0id,
		const MainMenuBar* mainMenuBar0,
		const ImGuiState* imGuiState0, 
		const ECSMenu* eCSMenu0,
		const ECSEditorMenuItem* eCSInspectorMenuItem0) {

		if (eCSMenu0->show_) {
			if (ImGui::MenuItem("Editor", "Ctrl+E")) {
				if (!IsComponentExist<ECSEditorWindow>(entity0id)) {
					CreateComponent<ECSEditorWindow>(entity0id);
				}
			}
		}

	};
}