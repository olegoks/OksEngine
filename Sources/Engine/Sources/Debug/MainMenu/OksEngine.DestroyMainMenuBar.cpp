#include <Debug/MainMenu\auto_OksEngine.DestroyMainMenuBar.hpp>

namespace OksEngine {
void DestroyMainMenuBar::Update(ECS2::Entity::Id entityId,
                                const ImGuiState* imGuiState,
                                const Input::HandleKeyboardEvents* handleKeyboardInput,
                                const Input::KeyboardEvents* keyboardInput,
                                const MainMenuBar* mainMenuBar) {
	//RemoveComponent<MainMenuBar>(entityId);
};
}