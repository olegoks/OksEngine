#include <Resources/auto_OksEngine.CheckResourceLoaded.hpp>


namespace OksEngine {

	void CheckResourceLoaded::Update(LoadResourceRequest* loadResourceRequest, AsyncTask* asyncTask) {

		//auto* asyncTask = world->GetComponent<AsyncTask>(entityId);
		//auto* name = world->GetComponent<Name>(entityId);
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
		//	const ECS::Entity::Id resourceEntity = world->CreateEntity();
		//	{
		//		world->CreateComponent<Resource>(resourceEntity);
		//		world->CreateComponent<Name>(resourceEntity, name->value_);
		//		world->CreateComponent<BinaryData>(resourceEntity,
		//			std::vector<Common::Byte>{
		//				getResourceResult.resource_.GetData<Common::Byte>(),
		//				getResourceResult.resource_.GetData<Common::Byte>() + getResourceResult.resource_.GetSize() } );
		//	}
		//	world->CreateComponent<ResourceEntity>(entityId, resourceEntity);
		//	world->CreateComponent<Completed>(entityId);
		//}


	}

	//std::string CheckResourceLoaded::GetName() const noexcept
	//{
	//	return "CheckResourceLoaded";
	//}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CheckResourceLoaded::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{
	//	ECS::Entity::Filter{}
	//	.Include<LoadResourceRequest>()
	//	.Include<AsyncTask>()
	//	.Exclude<BinaryData>()
	//	.Exclude<Completed>(),
	//	ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId CheckResourceLoaded::GetTypeId() const noexcept {
	//	return Common::TypeInfo<CheckResourceLoaded>().GetId();
	//}

	//CheckResourceLoaded::CheckResourceLoaded(Context& context) noexcept :
	//	ECSSystem{ context } { }

}