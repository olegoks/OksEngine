
#include <Render/GeometryDescriptionFile/OksEngine.CreateLoadGeometryDescriptionFileRequest.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/GeometryDescriptionFile/OksEngine.LoadGeometryDescriptionFileRequest.hpp>
#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFile.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>

#include <Common/auto_OksEngine.Name.hpp>
namespace OksEngine {



	void CreateLoadGeometryDescriptionFileRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		
		const ECS::Entity::Id loadResourceEntity = world->CreateEntity();
		{
			world->CreateComponent<LoadResourceRequest>(loadResourceEntity/*, immutableRenderGeometry->meshTag_*/);
			world->CreateComponent<Name>(loadResourceEntity, immutableRenderGeometry->meshTag_);
		}
		world->CreateComponent<LoadGeometryDescriptionFileRequest>(entityId, loadResourceEntity);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadGeometryDescriptionFileRequest::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Exclude<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateLoadGeometryDescriptionFileRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadGeometryDescriptionFileRequest>().GetId();
	}

}