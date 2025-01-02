#include <Resources/OksEngine.CreateAsyncTask.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Common/auto_OksEngine.Name.hpp>
#include <Resources/OksEngine.Subsystem.hpp>
#include <Resources/OksEngine.AsyncTask.hpp>
#include <Resources/OksEngine.WaitingForResourceLoadingByAnotherRequest.hpp>
#include <Resources/OksEngine.ResourceWasLoadedEarly.hpp>

namespace OksEngine {

	void CreateAsyncTask::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadResourceRequestMarker = world->GetComponent<LoadResourceRequest>(entityId);
		auto* resourceName = world->GetComponent<Name>(entityId);
		auto* subsystem = world->GetComponent<ECSResourceSubsystem>(secondEntityId);
		if (subsystem->loading_.contains(resourceName->value_)) {
			world->CreateComponent<WaitingForResourceLoading>(entityId);
		}
		else if(subsystem->loaded_.contains(resourceName->value_)) {
			world->CreateComponent<ResourceWasLoadedEarly>(entityId);
		}
		else {
			auto asyncTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, "Root\\" + std::string{ resourceName->value_ });
			world->CreateComponent<AsyncTask>(entityId, asyncTaskId);
			subsystem->loading_.insert(resourceName->value_);
		}
	}

	std::string CreateAsyncTask::GetName() const noexcept
	{
		return "CreateAsyncTask";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateAsyncTask::GetFilter() const noexcept {
		std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
		{
		ECS::Entity::Filter{}
		.Include<LoadResourceRequest>()
		.Include<Name>()
		.Exclude<AsyncTask>()
		.Exclude<WaitingForResourceLoading>(),
		ECS::Entity::Filter{}.Include<ECSResourceSubsystem>() };
		return filter;
	}

	Common::TypeId CreateAsyncTask::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateAsyncTask>().GetId();
	}

	CreateAsyncTask::CreateAsyncTask(Context& context) noexcept :
		ECSSystem{ context } { }

}