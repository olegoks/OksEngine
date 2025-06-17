#include <Debug\auto_OksEngine.DestroyImGuiState.hpp>

namespace OksEngine {
void DestroyImGuiState::Update(
	ECS2::Entity::Id entity0id,
	const ImGuiState* imGuiState0,
	const KeyboardInput* keyboardInput0) {

                                
		if (!IsEntityExist<ImGuiState>()) {
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

		DestroyEntity(entity0id);

                               };
}