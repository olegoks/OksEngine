#include <Render/GDF/auto_OksEngine.CheckGDFLoaded.hpp>

namespace OksEngine {

	void CheckGDFLoaded::Update(
		ECS2::Entity::Id entity1Id,
		const LoadGDFTask* loadGDFTask,
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

		AsyncResourceSubsystem::Task task;
		AsyncResourceSubsystem::Task::Id waitTaskId = loadGDFTask->id_;
		const bool isGot = resourceSystem->system_->IsIncomeTaskExist(Subsystem::Type::ChildThread, [waitTaskId](
			Subsystem::Type sender,
			const DS::Vector<Subsystem::Type>& receivers,
			const AsyncResourceSubsystem::Task& task) {
				return (waitTaskId == task.GetId());
			});
		if (isGot) {
			CreateComponent<GDFLoaded>(entity1Id); 

		}
		



	}

}