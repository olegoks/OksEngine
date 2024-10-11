
#include <Render/Model/ObjMtl/OksEngine.CreateMtl.hpp>

#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFile.hpp>
#include <Render/Model/ObjMtl/OksEngine.LoadMtlRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/Model/ObjMtl/OksEngine.MtlEntity.hpp>

#include <Common/OksEngine.Completed.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Common/OksEngine.BinaryData.hpp>

#include <Resources/OksEngine.ResourceEntity.hpp>

namespace OksEngine {


	void CreateMtlEntity::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {


		auto* loadMtlRequest = world->GetComponent<LoadMtlRequest>(entityId);
		const ECS::Entity::Id loadResourceRequestEntityId = loadMtlRequest->loadResourceRequestEntityId_;
		if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
			const ECS::Entity::Id resourceEntityId = resourceEntity->id_;
			auto* name = world->GetComponent<Name>(resourceEntityId);
			auto* binaryData = world->GetComponent<BinaryData>(resourceEntityId);
			const ECS::Entity::Id mtlEntity = world->CreateEntity();
			{
				world->CreateComponent<Name>(mtlEntity, name->value_);
				world->CreateComponent<BinaryData>(mtlEntity, binaryData->data_);
			}
			world->CreateComponent<MtlEntity>(entityId, mtlEntity);
		}
	}


	[[nodiscard]]
	std::string CreateMtlEntity::GetName() const noexcept {
		return "CreateMtlEntity";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateMtlEntity::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<LoadMtlRequest>()
			.Exclude<MtlEntity>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateMtlEntity::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateMtlEntity>().GetId();
	}

}