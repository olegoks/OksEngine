#include <Debug/ECS/Inspector\auto_OksEngine.CreateECSInspectorMenuItem.hpp>

namespace OksEngine {
void CreateECSInspectorMenuItem::Update(ECS2::Entity::Id entityId,
                                        const ImGUI::State* imGuiState,
                                        const MainMenuBar* mainMenuBar,
                                        const ECSMenu* eCSMenu) {

    CreateComponent<ECSInspectorMenuItem>(entityId);

};
}