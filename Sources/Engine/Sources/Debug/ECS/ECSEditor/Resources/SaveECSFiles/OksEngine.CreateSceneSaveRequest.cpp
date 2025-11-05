#include <Debug/ECS/ECSEditor/Resources\SaveECSFiles/auto_OksEngine.CreateSceneSaveRequest.hpp>

namespace OksEngine {
void CreateSceneSaveRequest::Update(
    ECS2::Entity::Id entity1Id, const ImGUI::State* imGuiState,
    const ECSEditorWindow* editorContext,
    const SaveECSFilesButtonPressed* loadECSFilesButtonPressed,
    ECS2::Entity::Id entity2Id, const ECSController* eCSController) {

    CreateComponent<SaveSceneRequest>(entity2Id);
    RemoveComponent<SaveECSFilesButtonPressed>(entity1Id);

};
}