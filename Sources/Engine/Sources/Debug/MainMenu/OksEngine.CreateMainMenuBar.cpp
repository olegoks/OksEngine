#include <Debug/MainMenu\auto_OksEngine.CreateMainMenuBar.hpp>

#include <UIAL.Window.hpp>

namespace OksEngine {
void CreateMainMenuBar::Update(ECS2::Entity::Id entityId,
                               const ImGuiState* imGuiState) {
			CreateComponent<MainMenuBar>(entityId);


};
}