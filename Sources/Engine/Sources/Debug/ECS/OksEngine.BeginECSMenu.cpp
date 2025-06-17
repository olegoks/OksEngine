#include <Debug/ECS\auto_OksEngine.BeginECSMenu.hpp>

namespace OksEngine {
void BeginECSMenu::Update(
	ECS2::Entity::Id entity0id, 
	const ImGuiState* imGuiState0,
	const MainMenuBar* mainMenuBar0,
	ECSMenu* eCSMenu0
						  ) {
		
        eCSMenu0->show_ = false;                    
		if (ImGui::BeginMenu("ECS")) {
			eCSMenu0->show_ = true;
		}

                          };
}