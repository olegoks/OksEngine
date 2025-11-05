#include <Debug/MainMenu\auto_OksEngine.EndMainMenuBar.hpp>

namespace OksEngine {
void EndMainMenuBar::Update(ECS2::Entity::Id entityId,
                            const MainMenuBar* mainMenuBar,
                            const ImGUI::State* imGuiState) {
                                
                                if(mainMenuBar->show_){
                                    ImGui::EndMainMenuBar();
                                }

                            };
}