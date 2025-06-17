#include <Debug\auto_OksEngine.CreateImGuiState.hpp>

namespace OksEngine {
	void CreateImGuiState::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const KeyboardInput* keyboardInput0) {

		if (IsEntityExist<ImGuiState>()) {
			return;
		}

		bool isKeyPressed = false;
		for (auto& event : keyboardInput0->events_) {
			if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
				isKeyPressed = true;
				break;
			}
		}

		if (!isKeyPressed) {
			return;
		}


		ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<ImGuiState>(entityId);
		CreateComponent<HandleKeyboardInput>(entityId);


	};
}