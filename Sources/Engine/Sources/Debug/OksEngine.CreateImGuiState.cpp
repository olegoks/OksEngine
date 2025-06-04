#include <Debug\auto_OksEngine.CreateImGuiState.hpp>

namespace OksEngine {
void CreateImGuiState::Update(
	ECS2::Entity::Id entity0id,
	const MainWindow* mainWindow0,
	const KeyboardInput* keyboardInput0) {

		ECS2::Entity::Id entityId = CreateEntity();

		for (auto& event : keyboardInput0->events_) {
			if (event.first == UI::Window::KeyboardKey::F5 && event.second == UI::Window::KeyboardAction::Pressed) {
				CreateComponent<ImGuiState>(entityId);
				CreateComponent<HandleKeyboardInput>(entityId);
				break;
			}
		}
		

};
}