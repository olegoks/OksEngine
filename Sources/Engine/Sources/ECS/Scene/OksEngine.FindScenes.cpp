#include <ECS\Scene\auto_OksEngine.FindScenes.hpp>

namespace OksEngine {
void FindScenes::Update(
    ECS2::Entity::Id entityId,
    ResourceSystem* resourceSystem) {


    auto taskId = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, { ".scn" });
    auto addedResource = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, taskId);

    std::vector<AsyncResourceSubsystem::Task::Id> getResourceTaskIds;

    for (auto& ecsFilePath : addedResource) {
        getResourceTaskIds.push_back(
            resourceSystem->system_->GetResource(
                Subsystem::Type::Debug, 
                ecsFilePath));
    }

    std::vector<Resources::ResourceData> scnResources;

    for (auto& taskId : getResourceTaskIds) {
        scnResources.push_back(resourceSystem->system_->GetResource(Subsystem::Type::Debug, taskId));
    }

    };
}