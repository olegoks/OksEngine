
#include <Render/OksEngine.CreateObj.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadObjRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.Obj.hpp>

namespace OksEngine {


	void CreateObj::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadObjRequest = world->GetComponent<LoadObjRequest>(entityId);
		if (world->IsComponentExist<Resource>(loadObjRequest->resourceTaskEntityId_)) {
			auto* resource = world->GetComponent<Resource>(loadObjRequest->resourceTaskEntityId_);
			world->CreateComponent<Obj>(entityId, resource->name_, resource->resourceData_.c_str());
		}
	}

	[[nodiscard]]
	std::string CreateObj::GetName() const noexcept {
		return "CreateObj";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateObj::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<LoadObjRequest>()
			.Exclude<Obj>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateObj::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateObj>().GetId();
	}

}