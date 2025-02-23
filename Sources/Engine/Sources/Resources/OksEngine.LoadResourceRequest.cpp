
//#include <Resources/OksEngine.LoadResourceRequest.hpp>
//
//#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	//void LoadResourceSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
	//	auto* request = world->GetComponent<LoadResourceRequest>(entityId);
	//	if (request->taskId_ == Common::Limits<Common::Index>::Max()) {
	//		request->taskId_ = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + request->resourceName_);
	//	} else {
	//		AsyncResourceSubsystem::Task task;
	//		AsyncResourceSubsystem::Task::Id waitTaskId = request->taskId_;
	//		const bool isGot = GetContext().GetResourceSubsystem()->GetTask(Subsystem::Type::Engine, task, [waitTaskId](
	//			Subsystem::Type sender,
	//			const DS::Vector<Subsystem::Type>& receivers,
	//			const AsyncResourceSubsystem::Task& task) { 
	//				return (waitTaskId == task.GetId());
	//			});
	//		if (isGot) {
	//			auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
	//			const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };
	//			world->CreateComponent<Resource>(entityId, request->resourceName_, resourceData);
	//		}
	//	}
	//}
	 
}