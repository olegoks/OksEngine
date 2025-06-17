#include <Debug/MainMenu\auto_OksEngine.BeginMainMenuBar.hpp>

namespace OksEngine {
void BeginMainMenuBar::Update(ECS2::Entity::Id entityId,
                              MainMenuBar* mainMenuBar,
                              const ImGuiState* imGuiState) {
                                mainMenuBar->show_ = false;
                                if(ImGui::BeginMainMenuBar()){
                                  mainMenuBar->show_ = true;
                                }

                              };
}