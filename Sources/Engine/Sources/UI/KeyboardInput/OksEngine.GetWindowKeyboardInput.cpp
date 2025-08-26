#include <UI\KeyboardInput/auto_OksEngine.GetWindowKeyboardInput.hpp>

namespace OksEngine {
void GetWindowKeyboardInput::Update(ECS2::Entity::Id entityId,
                                    MainWindow* mainWindow,
                                    KeyboardInput* keyboardInput) {
    
    while (auto event = mainWindow->window_->GetKeyboardEvent()) {
        static bool isCursorEnabled = false;
        //Debug code
        if (event.has_value()) {
            if (event.value().key_ == UIAL::Window::KeyboardKey::F5 && event.value().event_ == UIAL::Window::KeyboardAction::Pressed) {

                if (isCursorEnabled) {
                    mainWindow->window_->DisableCursor();
                    isCursorEnabled = false;
                }
                else {
                    mainWindow->window_->EnableCursor();
                    isCursorEnabled = true;
                }
                
            }
        }
        if (isCursorEnabled) {
            continue;
        }
        keyboardInput->events_.push_back({ event.value().key_, event.value().event_ });
    }
    
};
}