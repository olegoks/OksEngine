#include <Resources/OksEngine.LoadResource.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Resources/OksEngine.AsyncTask.hpp>

namespace OksEngine {

	void CreateAsyncTask::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadResourceRequestMarker = world->GetComponent<LoadResourceRequest>(entityId);
		auto* resourceName = world->GetComponent<Name>(entityId);

		//auto* request = world->GetComponent<LoadResourceRequest>(entityId);
		//if (request->taskId_ == Common::Limits<Common::Index>::Max()) {
		//	request->taskId_ = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + request->resourceName_);
		//}
		//else {
		//	AsyncResourceSubsystem::Task task;
		//	AsyncResourceSubsystem::Task::Id waitTaskId = request->taskId_;
		//	const bool isGot = GetContext().GetResourceSubsystem()->GetTask(Subsystem::Type::Engine, task, [waitTaskId](
		//		Subsystem::Type sender,
		//		const DS::Vector<Subsystem::Type>& receivers,
		//		const AsyncResourceSubsystem::Task& task) {
		//			return (waitTaskId == task.GetId());
		//		});
		//	if (isGot) {
		//		auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
		//		const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };
		//		world->CreateComponent<Resource>(entityId, request->resourceName_, resourceData);
		//	}
		//}

		auto asyncTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + std::string{ resourceName->value_ });
		world->CreateComponent<AsyncTask>(entityId, asyncTaskId);

	}

	std::string CreateAsyncTask::GetName() const noexcept
	{
		return "LoadResource";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateAsyncTask::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<LoadResourceRequest>()
			.Include<Name>(),
			ECS::Entity::Filter{}.ExcludeAll()
			/*.Include<LoadResourceRequest>()
			.Exclude<Resource>()*/ };
	}

	Common::TypeId CreateAsyncTask::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateAsyncTask>().GetId();
	}

	CreateAsyncTask::CreateAsyncTask(Context& context) noexcept :
		ECSSystem{ context } { }

}