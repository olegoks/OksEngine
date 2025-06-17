#include <Debug/ECS\auto_OksEngine.EndECSMenu.hpp>

namespace OksEngine {
void EndECSMenu::Update(ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
                        const MainMenuBar* mainMenuBar, const ECSMenu* ecsMenu) {
                            if(ecsMenu->show_){
                                ImGui::EndMenu();
                                
                            }
                        };
}