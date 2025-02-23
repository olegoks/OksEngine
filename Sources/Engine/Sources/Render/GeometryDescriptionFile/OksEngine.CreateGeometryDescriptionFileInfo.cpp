
#include <Render/GeometryDescriptionFile/auto_OksEngine.CreateGeometryDescriptionFileInfo.hpp>

namespace OksEngine {

	void CreateGeometryDescriptionFileInfo::Update(
		ECS2::Entity::Id entity1Id,
		const GeometryDescriptionFileLoaded* geometryDescriptionFileLoaded,
		const LoadGeometryDescriptionFileTask* loadGeometryDescriptionFileTask,
		ECS2::Entity::Id entity2Id,
		ResourceSystem* resourceSystem) {

		const ECS2::Entity::Id loadResourceRequestEntityId = loadGeometryDescriptionFileTask->id_;

		AsyncResourceSubsystem::Task task;
		AsyncResourceSubsystem::Task::Id waitTaskId = loadGeometryDescriptionFileTask->id_;
		const bool isGot = resourceSystem->system_->GetTask(Subsystem::Type::Engine, task, [waitTaskId](
			Subsystem::Type sender,
			const DS::Vector<Subsystem::Type>& receivers,
			const AsyncResourceSubsystem::Task& task) {
				return (waitTaskId == task.GetId());
			});

		auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
		const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };

		CreateComponent<GeometryDescriptionFileInfo>(entity1Id, resourceData);
	}

}