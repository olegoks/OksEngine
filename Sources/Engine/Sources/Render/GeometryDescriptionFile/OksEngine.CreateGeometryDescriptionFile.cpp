
#include <Render/GeometryDescriptionFile/auto_OksEngine.CreateGeometryDescriptionFileEntity.hpp>

namespace OksEngine {

	void CreateGeometryDescriptionFileEntity::Update(ImmutableRenderGeometry* immutableRenderGeometry, LoadGeometryDescriptionFileRequest* loadGeometryDescriptionFileRequest) {
		/*auto* loadGeometryDescriptionFileRequest = world->GetComponent<LoadGeometryDescriptionFileRequest>(entityId);
		const ECS::Entity::Id loadResourceRequestEntityId = loadGeometryDescriptionFileRequest->loadResourceRequestEntityId_;
		if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
			const ECS::Entity::Id resourceEntityId = resourceEntity->id_;
			{
				auto* name = world->GetComponent<Name>(resourceEntityId);
				auto* binaryData = world->GetComponent<BinaryData>(resourceEntityId);
				const ECS::Entity::Id geometryDescriptionFileRequest = world->CreateEntity();
				{
					world->CreateComponent<Name>(geometryDescriptionFileRequest, name->value_);
					world->CreateComponent<Text>(geometryDescriptionFileRequest, std::string{ binaryData->data_.data(), binaryData->data_.size() });
				}
				world->CreateComponent<GeometryDescriptionFileEntity>(entityId, geometryDescriptionFileRequest);
			}
		}*/
	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateGeometryDescriptionFileEntity::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<ImmutableRenderGeometry>()
	//		.Exclude<GeometryDescriptionFileEntity>()
	//		.Include<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}