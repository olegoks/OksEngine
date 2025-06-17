#include <Debug/ECS/Inspector/Window\auto_OksEngine.BeginECSInspectorWindow.hpp>

namespace OksEngine {
void BeginECSInspectorWindow::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const MainMenuBar* mainMenuBar, const ECSMenu* eCSMenu,
    const ECSInspectorMenuItem* eCSInspectorMenuItem,
    ECSInspectorWindow* eCSInspectorWindow) {
    
    ImGui::Begin("ECS Inspector");


};
}