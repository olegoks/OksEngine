#include <Debug/MainMenu\auto_OksEngine.DestroyMainMenuBar.hpp>

namespace OksEngine {
void DestroyMainMenuBar::Update(ECS2::Entity::Id entityId,
                                const ImGuiState* imGuiState,
                                const HandleKeyboardInput* handleKeyboardInput,
                                const KeyboardInput* keyboardInput,
                                const MainMenuBar* mainMenuBar) {
	for (auto& event : keyboardInput->events_) {
		if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
			RemoveComponent<MainMenuBar>(entityId);
			break;
		}
	}
};
}