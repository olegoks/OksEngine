#include <Debug/ECS/ECSEditor\auto_OksEngine.CreateECSEditorMenuItem.hpp>

namespace OksEngine {
void CreateECSEditorMenuItem::Update(ECS2::Entity::Id entityId,
                                        const ImGUI::State* imGuiState,
                                        const MainMenuBar* mainMenuBar,
                                        const ECSMenu* eCSMenu) {

    CreateComponent<ECSEditorMenuItem>(entityId);

};
}