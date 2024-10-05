
#include <Render/OksEngine.CreateLoadObjRequest.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadObjRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>

namespace OksEngine {


	void CreateLoadObjRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* geomFile = world->GetComponent<GeometryFile>(entityId);

		if (geomFile->mesh_["Obj"]) {

			const std::string objName = geomFile->mesh_["Obj"].as<std::string>();
			const ECS::Entity::Id loadObjResourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(loadObjResourceEntityId, objName);

			world->CreateComponent<LoadObjRequest>(entityId, loadObjResourceEntityId);

		}
	}

	[[nodiscard]]
	std::string CreateLoadObjRequest::GetName() const noexcept {
		return "CreateLoadObjRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadObjRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<GeometryFile>()
			.Exclude<LoadObjRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadObjRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadObjRequest>().GetId();
	}

}