#include <Resources/OksEngine.CreateResourceEntity.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Resources/OksEngine.AsyncTask.hpp>
#include <Resources/OksEngine.ResourceEntity.hpp>

namespace OksEngine {

	void CreateResourceEntity::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadResourceRequest = world->GetComponent<LoadResourceRequest>(entityId);
		auto* resourceName = world->GetComponent<Name>(entityId);
		auto* asyncTask = world->GetComponent<AsyncTask>(entityId);

		AsyncResourceSubsystem::Task task;
		AsyncResourceSubsystem::Task::Id waitTaskId = asyncTask->id_;
		const bool isGot = GetContext().GetResourceSubsystem()->GetTask(Subsystem::Type::Engine, task, [waitTaskId](
			Subsystem::Type sender,
			const DS::Vector<Subsystem::Type>& receivers,
			const AsyncResourceSubsystem::Task& task) {
				return (waitTaskId == task.GetId());
			});
		if (isGot) {
			auto getResourceResult = std::move(task.GetData<AsyncResourceSubsystem::GetResourceResult>());
			const std::string resourceData{ getResourceResult.resource_.GetData<char>(), getResourceResult.resource_.GetSize() };

			ECS::Entity::Id resourceEntity = world->CreateEntity();
			world->CreateComponent<Resource>(resourceEntity, resourceData);

			world->CreateComponent<ResourceEntity>(entityId, resourceEntity);
		}
	}

	std::string CreateResourceEntity::GetName() const noexcept
	{
		return "CreateResourceEntity";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateResourceEntity::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<LoadResourceRequest>()
			.Include<Name>()
			.Include<AsyncTask>(),
			ECS::Entity::Filter{}.Exclude<ResourceEntity>() };
	}

	Common::TypeId CreateResourceEntity::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateResourceEntity>().GetId();
	}

	CreateResourceEntity::CreateResourceEntity(Context& context) noexcept :
		ECSSystem{ context } { }

}