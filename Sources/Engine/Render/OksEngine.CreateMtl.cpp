
#include <Render/OksEngine.CreateMtl.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadMtlRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.Mtl.hpp>

namespace OksEngine {


	void CreateMtl::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadMtlRequest = world->GetComponent<LoadMtlRequest>(entityId);
		if (world->IsComponentExist<Resource>(loadMtlRequest->resourceTaskEntityId_)) {
			auto* resource = world->GetComponent<Resource>(loadMtlRequest->resourceTaskEntityId_);
			world->CreateComponent<Mtl>(entityId, resource->name_, resource->resourceData_.c_str());
		}
	}

	[[nodiscard]]
	std::string CreateMtl::GetName() const noexcept {
		return "CreateMtl";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateMtl::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<LoadMtlRequest>()
			.Exclude<Mtl>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateMtl::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateMtl>().GetId();
	}

}