#include <Resources/OksEngine.WaitForResourceLoading.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Resources/OksEngine.Subsystem.hpp>
#include <Resources/OksEngine.AsyncTask.hpp>
#include <Resources/OksEngine.WaitingForResourceLoadingByAnotherRequest.hpp>
#include <Resources/OksEngine.ResourceWasLoadedEarly.hpp>
#include <Resources/OksEngine.ResourceEntity.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Common/OksEngine.BinaryData.hpp>
#include <Common/OksEngine.Completed.hpp>

namespace OksEngine {

	void WaitForResourceLoading::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* resourceName = world->GetComponent<Name>(entityId);
		if (resourceName->value_ == world->GetComponent<Name>(secondEntityId)->value_) {
			world->CreateComponent<ResourceEntity>(entityId, secondEntityId);
			world->CreateComponent<Completed>(entityId);
		}
	}

	std::string WaitForResourceLoading::GetName() const noexcept
	{
		return "WaitForResourceLoading";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> WaitForResourceLoading::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<LoadResourceRequest>()
			.Include<Name>()
			.Include<WaitingForResourceLoading>()
			.Exclude<ResourceEntity>(),
			ECS::Entity::Filter{}
		.Include<Name>()
			.Include<Resource>()
		.Include<BinaryData>() };
	}

	Common::TypeId WaitForResourceLoading::GetTypeId() const noexcept {
		return Common::TypeInfo<WaitForResourceLoading>().GetId();
	}

	WaitForResourceLoading::WaitForResourceLoading(Context& context) noexcept :
		ECSSystem{ context } { }

}