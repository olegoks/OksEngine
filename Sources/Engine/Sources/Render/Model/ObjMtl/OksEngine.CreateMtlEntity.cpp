
#include <Render/Model/ObjMtl/auto_OksEngine.CreateMtlEntity.hpp>


namespace OksEngine {


	void CreateMtlEntity::Update(LoadMtlRequest* loadMtlRequest) {


		//auto* loadMtlRequest = world->GetComponent<LoadMtlRequest>(entityId);
		//const ECS::Entity::Id loadResourceRequestEntityId = loadMtlRequest->loadResourceRequestEntityId_;
		//if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
		//	auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
		//	const ECS::Entity::Id resourceEntityId = resourceEntity->id_;
		//	auto* name = world->GetComponent<Name>(resourceEntityId);
		//	auto* binaryData = world->GetComponent<BinaryData>(resourceEntityId);
		//	const ECS::Entity::Id mtlEntity = world->CreateEntity();
		//	{
		//		world->CreateComponent<Name>(mtlEntity, name->value_);
		//		world->CreateComponent<BinaryData>(mtlEntity, binaryData->data_);
		//	}
		//	world->CreateComponent<MtlEntity>(entityId, mtlEntity);
		//}
	}

}