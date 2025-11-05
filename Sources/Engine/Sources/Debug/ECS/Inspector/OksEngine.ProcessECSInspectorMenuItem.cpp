#include <Debug/ECS/Inspector\auto_OksEngine.ProcessECSInspectorMenuItem.hpp>

namespace OksEngine {
	void ProcessECSInspectorMenuItem::Update(
		ECS2::Entity::Id entity0id,
		const MainMenuBar* mainMenuBar0,
		const ImGUI::State* imGuiState0, 
		const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0) {

		if (eCSMenu0->show_) {
			if (ImGui::MenuItem("Inspector", "Ctrl+E")) {
				if (!IsComponentExist<ECSInspectorWindow>(entity0id)) {
					CreateComponent<ECSInspectorWindow>(entity0id);
				}
			}
		}

	};
}