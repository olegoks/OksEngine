#include <UI\KeyboardInput/auto_OksEngine.GetWindowKeyboardInput.hpp>

namespace OksEngine {
void GetWindowKeyboardInput::Update(ECS2::Entity::Id entityId,
                                    MainWindow* mainWindow,
                                    KeyboardInput* keyboardInput) {
    
    while (auto event = mainWindow->window_->GetKeyboardEvent()) {
        keyboardInput->events_.push_back({ event.value().key_, event.value().event_ });
    }
    
};
}