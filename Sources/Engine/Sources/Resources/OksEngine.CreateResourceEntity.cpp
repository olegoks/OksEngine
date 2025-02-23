#include <Resources/auto_OksEngine.CreateResourceEntity.hpp>

#include <Resources/OksEngine.Subsystem.hpp>

namespace OksEngine {

	void CreateResourceEntity::Update(
		ECS2::Entity::Id entityId,
		const LoadResourceRequest* loadResourceRequest1, 
		const Name* name1, 
		const AsyncTask* asyncTask1) {

		//auto* loadResourceRequest = world->GetComponent<LoadResourceRequest>(entityId);
		//auto* resourceName = world->GetComponent<Name>(entityId);
		//auto* asyncTask = world->GetComponent<AsyncTask>(entityId);

		//AsyncResourceSubsystem::Task task;
		//AsyncResourceSubsystem::Task::Id waitTaskId = asyncTask->id_;
		//const bool isGot = GetContext().GetResourceSubsystem()->GetTask(Subsystem::Type::Engine, task, [waitTaskId](
		//	Subsystem::Type sender,
		//	const DS::Vector<Subsystem::Type>& receivers,
		//	const AsyncResourceSubsystem::Task& task) {
		//		return (waitTaskId == task.GetId());
		//	});
		//if (isGot) {
		//	auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
		//	const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };

		//	ECS::Entity::Id resourceEntity = world->CreateEntity();
		//	world->CreateComponent<BinaryData>(resourceEntity, std::vector<Common::Byte>{ getResourceResult.resource_.GetData<Common::Byte>(), getResourceResult.resource_.GetData<Common::Byte>() + getResourceResult.resource_.GetSize()  });
		//	world->CreateComponent<ResourceEntity>(entityId, resourceEntity);
		//	world->CreateComponent<Resource>(entityId);
		//	auto* subsystem = world->GetComponent<ECSResourceSubsystem>(secondEntityId);
		//	subsystem->loading_.erase(resourceName->value_);
		//	subsystem->loaded_.insert(resourceName->value_);
		//}
	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateResourceEntity::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{
	//	ECS::Entity::Filter{}
	//	.Include<LoadResourceRequest>()
	//	.Include<Name>()
	//	.Include<AsyncTask>()
	//	.Exclude<ResourceEntity>(),
	//	ECS::Entity::Filter{}.Include<ECSResourceSubsystem>() };
	//	return filter;
	//}

}