#include <Debug/ECSCallGraphEditor/Resources\auto_OksEngine.CreateNodeEntities.hpp>

namespace OksEngine {
void CreateNodeEntities::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const EditorContext* editorContext,
    const LoadECSFilesButtonPressed* loadECSFilesButtonPressed,
    ECS2::Entity::Id entity2Id, ResourceSystem* resourceSystem) {

    auto taskId = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, { ".ecs" });
    auto addedResource = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, taskId);


};
}