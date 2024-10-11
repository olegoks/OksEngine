#include <Resources/OksEngine.CreateResourceEntity.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Common/OksEngine.BinaryData.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Resources/OksEngine.AsyncTask.hpp>
#include <Resources/OksEngine.ResourceEntity.hpp>
#include <Resources/OksEngine.Subsystem.hpp>

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
			world->CreateComponent<BinaryData>(resourceEntity, std::vector<Common::Byte>{ getResourceResult.resource_.GetData<Common::Byte>(), getResourceResult.resource_.GetData<Common::Byte>() + getResourceResult.resource_.GetSize()  });
			world->CreateComponent<ResourceEntity>(entityId, resourceEntity);
			world->CreateComponent<Resource>(entityId);
			auto* subsystem = world->GetComponent<ECSResourceSubsystem>(secondEntityId);
			subsystem->loading_.erase(resourceName->value_);
			subsystem->loaded_.insert(resourceName->value_);
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
			.Include<AsyncTask>()
			.Exclude<ResourceEntity>(),
			ECS::Entity::Filter{}.Include<ECSResourceSubsystem>() };
	}

	Common::TypeId CreateResourceEntity::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateResourceEntity>().GetId();
	}

	CreateResourceEntity::CreateResourceEntity(Context& context) noexcept :
		ECSSystem{ context } { }

}