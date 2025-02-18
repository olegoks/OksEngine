
#include <Render/GeometryDescriptionFile/auto_OksEngine.CreateLoadGeometryDescriptionFileRequest.hpp>

namespace OksEngine {



	void CreateLoadGeometryDescriptionFileRequest::Update(ImmutableRenderGeometry* immutableRenderGeometry) {

		//const ECS2::Entity::Id loadResourceEntity = world->CreateEntity();
		//{
		//	world->CreateComponent<LoadResourceRequest>(loadResourceEntity/*, immutableRenderGeometry->meshTag_*/);
		//	world->CreateComponent<Name>(loadResourceEntity, immutableRenderGeometry->meshTag_);
		//}
		//world->CreateComponent<LoadGeometryDescriptionFileRequest>(entityId, loadResourceEntity);
	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadGeometryDescriptionFileRequest::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<ImmutableRenderGeometry>()
	//		.Exclude<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}