
#include <Render/OksEngine.CreateLoadGeometryDescriptionFileRequest.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/OksEngine.LoadGeometryDescriptionFileRequest.hpp>
#include <Render/OksEngine.GeometryFile.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>

namespace OksEngine {



	void CreateLoadGeometryDescriptionFileRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		
		const ECS::Entity::Id loadResourceEntity = world->CreateEntity();
		world->CreateComponent<LoadResourceRequest>(loadResourceEntity, immutableRenderGeometry->meshTag_);

		world->CreateComponent<LoadGeometryDescriptionFileRequest>(entityId, loadResourceEntity);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadGeometryDescriptionFileRequest::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Exclude<GeometryFile>()
			.Exclude<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadGeometryDescriptionFileRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadGeometryDescriptionFileRequest>().GetId();
	}

}