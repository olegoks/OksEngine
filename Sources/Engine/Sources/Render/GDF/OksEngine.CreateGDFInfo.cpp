
#include <Render/GDF/auto_OksEngine.CreateGDFInfo.hpp>

namespace OksEngine {

	void CreateGDFInfo::Update(
		ECS2::Entity::Id entity1Id,
		const GDFLoaded* GDFLoaded,
		const LoadGDFTask* loadGDFTask,
		ECS2::Entity::Id entity2Id,
		ResourceSystem* resourceSystem) {

		const ECS2::Entity::Id loadResourceRequestEntityId = loadGDFTask->id_;

		AsyncResourceSubsystem::Task task;
		AsyncResourceSubsystem::Task::Id waitTaskId = loadGDFTask->id_;
		const bool isGot = resourceSystem->system_->GetTask(Subsystem::Type::ChildThread, task, [waitTaskId](
			Subsystem::Type sender,
			const DS::Vector<Subsystem::Type>& receivers,
			const AsyncResourceSubsystem::Task& task) {
				return (waitTaskId == task.GetId());
			});

		auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
		const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };

		CreateComponent<GDFInfo>(entity1Id, resourceData);
	}

}