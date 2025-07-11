
#include <Render/GDF/auto_OksEngine.CreateLoadGDFTask.hpp>

namespace OksEngine {

	void CreateLoadGDFTask::Update(
		ECS2::Entity::Id entity1Id,
		const ImmutableRenderGeometry* immutableRenderGeometry,
		ECS2::Entity::Id entity2Id,
		ResourceSystem* resourceSystem) {

		//auto* loadResourceRequestMarker = world->GetComponent<LoadResourceRequest>(entityId);
		//auto* resourceName = world->GetComponent<Name>(entityId);
		//auto* subsystem = world->GetComponent<ECSResourceSubsystem>(secondEntityId);
		//if (subsystem->loading_.contains(resourceName->value_)) {
		//	world->CreateComponent<WaitingForResourceLoading>(entityId);
		//}
		//else if (subsystem->loaded_.contains(resourceName->value_)) {
		//	world->CreateComponent<ResourceWasLoadedEarly>(entityId);
		//}
		//else {
		//	auto asyncTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + std::string{ resourceName->value_ });
		//	world->CreateComponent<AsyncTask>(entityId, asyncTaskId);
		//	subsystem->loading_.insert(resourceName->value_);
		//}
		auto asyncTaskId = resourceSystem->system_->GetResource(Subsystem::Type::ChildThread, "Root\\" + std::string{ immutableRenderGeometry->meshTag_ });

		CreateComponent<LoadGDFTask>(entity1Id, asyncTaskId);
	}


}