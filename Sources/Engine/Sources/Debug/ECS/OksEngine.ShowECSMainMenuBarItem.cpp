#include <Debug/ECS\auto_OksEngine.ShowECSMainMenuBarItem.hpp>

namespace OksEngine {
	void ShowECSMainMenuBarItem::Update(ECS2::Entity::Id entityId,
		const ImGuiState* imGuiState,
		const MainMenuBar* mainMenuBar) {

		if (ImGui::BeginMenu("ECS")) {
			ImGui::EndMenu();
		}
	};
}