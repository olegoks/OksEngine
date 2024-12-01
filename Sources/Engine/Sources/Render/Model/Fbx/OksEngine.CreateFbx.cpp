
#include <Render/Model/Fbx/OksEngine.CreateFbx.hpp>
//#include <Render/Model/Fbx/>

#include <Render/OksEngine.Render.Components.hpp>
#include <Resources/OksEngine.Resources.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

namespace OksEngine {


	void CreateFbxEntity::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadFbxRequest = world->GetComponent<LoadFbxRequest>(entityId);
		const ECS::Entity::Id loadResourceRequestEntityId = loadFbxRequest->loadResourceRequestEntityId_;
		if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
			const ECS::Entity::Id resourceEntityId = resourceEntity->id_;
			auto* name = world->GetComponent<Name>(resourceEntityId);
			auto* binaryData = world->GetComponent<BinaryData>(resourceEntityId);
			const ECS::Entity::Id objEntity = world->CreateEntity();
			{
				world->CreateComponent<Name>(objEntity, name->value_);
				world->CreateComponent<BinaryData>(objEntity, binaryData->data_);
			}
			world->CreateComponent<FbxEntity>(entityId, objEntity);
		}
	}

	[[nodiscard]]
	std::string CreateFbxEntity::GetName() const noexcept {
		return "CreateFbxEntity";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateFbxEntity::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<LoadFbxRequest>()
			.Exclude<FbxEntity>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateFbxEntity::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateFbxEntity>().GetId();
	}

}