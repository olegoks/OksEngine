#include <Debug\auto_OksEngine.EndImGuiFrame.hpp>


#include <imgui.h>

namespace OksEngine {
void EndImGuiFrame::Update(ECS2::Entity::Id entityId,
                           const ImGuiState* imGuiState) {

    ImGui::Render();

};
}                                                                     