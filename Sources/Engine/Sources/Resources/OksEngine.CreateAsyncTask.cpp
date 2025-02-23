#include <Resources/auto_OksEngine.CreateAsyncTask.hpp>

#include <Resources/OksEngine.Subsystem.hpp>

namespace OksEngine {

	void CreateAsyncTask::Update(
		ECS2::Entity::Id entityId, 
		const LoadResourceRequest* loadResourceRequest,
		const Name* name1) {

		//auto* loadResourceRequestMarker = world->GetComponent<LoadResourceRequest>(entityId);
		//auto* resourceName = world->GetComponent<Name>(entityId);
		//auto* subsystem = world->GetComponent<ECSResourceSubsystem>(secondEntityId);
		//if (subsystem->loading_.contains(resourceName->value_)) {
		//	world->CreateComponent<WaitingForResourceLoading>(entityId);
		//}
		//else if(subsystem->loaded_.contains(resourceName->value_)) {
		//	world->CreateComponent<ResourceWasLoadedEarly>(entityId);
		//}
		//else {
		//	auto asyncTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + std::string{ resourceName->value_ });
		//	world->CreateComponent<AsyncTask>(entityId, asyncTaskId);
		//	subsystem->loading_.insert(resourceName->value_);
		//}
	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateAsyncTask::GetFilter() const noexcept {
	//	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{
	//	ECS::Entity::Filter{}
	//	.Include<LoadResourceRequest>()
	//	.Include<Name>()
	//	.Exclude<AsyncTask>()
	//	.Exclude<WaitingForResourceLoading>(),
	//	ECS::Entity::Filter{}.Include<ECSResourceSubsystem>() };
	//	return filter;
	//}

}