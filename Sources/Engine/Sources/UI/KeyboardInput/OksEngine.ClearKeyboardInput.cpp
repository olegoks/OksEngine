#include <UI\KeyboardInput/auto_OksEngine.ClearKeyboardInput.hpp>

namespace OksEngine {
void ClearKeyboardInput::Update(ECS2::Entity::Id entityId,
                                KeyboardInput* keyboardInput) {

keyboardInput->events_.clear();

                                };
}