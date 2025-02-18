
#include <Render/Model/ObjMtl/auto_OksEngine.CreateObjEntity.hpp>

namespace OksEngine {


	void CreateObjEntity::Update(LoadObjRequest* loadObjRequest) {

		//auto* loadObjRequest = world->GetComponent<LoadObjRequest>(entityId);
		//const ECS::Entity::Id loadResourceRequestEntityId = loadObjRequest->loadResourceRequestEntityId_;
		//if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
		//	auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
		//	const ECS::Entity::Id resourceEntityId = resourceEntity->id_;
		//	auto* name = world->GetComponent<Name>(resourceEntityId);
		//	auto* binaryData = world->GetComponent<BinaryData>(resourceEntityId);
		//	const ECS::Entity::Id objEntity = world->CreateEntity();
		//	{
		//		world->CreateComponent<Name>(objEntity, name->value_);
		//		world->CreateComponent<BinaryData>(objEntity, binaryData->data_);
		//	}
		//	world->CreateComponent<ObjEntity>(entityId, objEntity);
		//}
	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateObjEntity::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<LoadObjRequest>()
	//		.Exclude<ObjEntity>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}


}