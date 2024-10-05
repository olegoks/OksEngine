
#include <Render/OksEngine.CreateLoadMtlRequest.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadMtlRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>


namespace OksEngine {


	void CreateLoadMtlRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* geomFile = world->GetComponent<GeometryFile>(entityId);

		if (geomFile->mesh_["Mtl"]) {

			const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
			const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId, mtlName);

			world->CreateComponent<LoadMtlRequest>(entityId, loadMtlResourceEntityId);

		}
	}

	[[nodiscard]]
	std::string CreateLoadMtlRequest::GetName() const noexcept {
		return "CreateLoadMtlRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadMtlRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<GeometryFile>()
			.Exclude<LoadMtlRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadMtlRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadMtlRequest>().GetId();
	}

}