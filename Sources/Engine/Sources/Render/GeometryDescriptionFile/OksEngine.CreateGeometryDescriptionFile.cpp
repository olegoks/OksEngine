
#include <Render/GeometryDescriptionFile/OksEngine.CreateGeometryDescriptionFile.hpp>

#include <Render/GeometryDescriptionFile/OksEngine.LoadGeometryDescriptionFileRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFile.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <Resources/OksEngine.ResourceEntity.hpp>
#include <Common/auto_OksEngine.Completed.hpp>
#include <Common/auto_OksEngine.Name.hpp>
#include <Common/auto_OksEngine.Text.hpp>
#include <Common/auto_OksEngine.BinaryData.hpp>

#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFileEntity.hpp>

namespace OksEngine {

	void CreateGeometryDescriptionFileEntity::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* loadGeometryDescriptionFileRequest = world->GetComponent<LoadGeometryDescriptionFileRequest>(entityId);
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
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateGeometryDescriptionFileEntity::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Exclude<GeometryDescriptionFileEntity>()
			.Include<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

}