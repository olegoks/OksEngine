#include <Debug\auto_OksEngine.ShowImGuiInterface.hpp>

namespace OksEngine {
void ShowImGuiInterface::Update(ECS2::Entity::Id entityId,
                                  const ImGuiState* imGuiState,
                                  KeyboardInput* keyboardInput) {

    for (auto& event : keyboardInput->events_) {
        if (event.first == UIAL::Window::KeyboardKey::F5) {
            CreateComponent<MainMenuBar>(entityId);
        }
    }

};
}