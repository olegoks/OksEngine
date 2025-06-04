#include <UI\auto_OksEngine.CreateKeyboardInput.hpp>

namespace OksEngine {
void CreateKeyboardInput::Update(
    ECS2::Entity::Id entityId, const HandleKeyboardInput* handleKeyboardInput) {

    CreateComponent<KeyboardInput>(entityId);

};
}