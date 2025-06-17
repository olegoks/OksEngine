#include <UI\KeyboardInput/auto_OksEngine.SendWindowKeyboardInput.hpp>

namespace OksEngine {
void SendWindowKeyboardInput::Update(
    ECS2::Entity::Id entity1Id, const MainWindow* mainWindow,
    const KeyboardInput* keyboardInput1, ECS2::Entity::Id entity2Id,
    const HandleKeyboardInput* handleKeyboardInput,
    KeyboardInput* keyboardInput2) {
    
    for (auto& event : keyboardInput1->events_) {
        keyboardInput2->events_.push_back(event);
    }

};
}