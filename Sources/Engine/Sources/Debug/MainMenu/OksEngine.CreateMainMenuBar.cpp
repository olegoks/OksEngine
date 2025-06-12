#include <Debug/MainMenu\auto_OksEngine.CreateMainMenuBar.hpp>

#include <UIAL.Window.hpp>

namespace OksEngine {
void CreateMainMenuBar::Update(ECS2::Entity::Id entityId,
                               const ImGuiState* imGuiState,
                               const HandleKeyboardInput* handleKeyboardInput,
                               const KeyboardInput* keyboardInput) {
	for(auto& event : keyboardInput->events_){
		if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
			CreateComponent<MainMenuBar>(entityId);
			break;
		}
	}


};
}