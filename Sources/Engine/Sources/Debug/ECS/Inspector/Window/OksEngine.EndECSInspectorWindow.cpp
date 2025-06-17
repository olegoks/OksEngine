#include <Debug/ECS/Inspector/Window\auto_OksEngine.EndECSInspectorWindow.hpp>

namespace OksEngine {
	void EndECSInspectorWindow::Update(
		ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
		const MainMenuBar* mainMenuBar, const ECSMenu* eCSMenu,
		const ECSInspectorMenuItem* eCSInspectorMenuItem,
		const ECSInspectorWindow* eCSInspectorWindow) {

		ImGui::End();

	};
}