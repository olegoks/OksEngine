#include <Debug/ECS\auto_OksEngine.CreateECSMenu.hpp>

namespace OksEngine {
void CreateECSMenu::Update(ECS2::Entity::Id entityId,
                           const ImGuiState* imGuiState,
                           const HandleKeyboardInput* handleKeyboardInput,
                           const KeyboardInput* keyboardInput) {

                            CreateComponent<ECSMenu>(entityId, false);

                           };
}