#include <Debug\auto_OksEngine.StartImGuiFrame.hpp>

#include <imgui.h>

namespace OksEngine {
void StartImGuiFrame::Update(ECS2::Entity::Id entityId,
                             const ImGuiState* imGuiState) {
    
    ImGui::NewFrame();
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text("Welcome to ImGui!");
    ImGui::Button("Click Me");
    ImGui::End();

};
}