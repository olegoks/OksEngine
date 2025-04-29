#include <Debug\auto_OksEngine.CreateImGuiInterface.hpp>

namespace OksEngine {
void CreateImGuiInterface::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    KeyboardInput* keyboardInput) {
    
    for (auto& event : keyboardInput->events_) {
        if (event.first == UIAL::Window::KeyboardKey::F5) {
            CreateComponent<MainMenuBar>(entityId);
        }
    }

};
}