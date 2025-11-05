#include <Debug/ECS\auto_OksEngine.CreateECSMenu.hpp>

namespace OksEngine {
void CreateECSMenu::Update(ECS2::Entity::Id entityId,
                           const ImGUI::State* imGuiState,
                           const Input::HandleKeyboardEvents* handleKeyboardInput,
                           const Input::KeyboardEvents* keyboardInput) {

                            CreateComponent<ECSMenu>(entityId, false);

                           };
}