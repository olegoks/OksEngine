
#include <Animation/auto_OksEngine.StartModelAnimation.hpp>


//
//#include <Common/auto_OksEngine.ChildEntity.hpp>
//
//#include <Common/auto_OksEngine.ChildEntities.hpp>

namespace OksEngine {

	//void StartNodeAnimation(ECS::World* world, ECS::Entity::Id nodeEntityId) {

	//	if (world->IsComponentExist<Animation>(nodeEntityId)) {
	//		world->CreateComponent<StartAnimation>(nodeEntityId);
	//	}

	//	if (world->IsComponentExist<ChildEntities>(nodeEntityId)) {
	//		auto* childEntities = world->GetComponent<ChildEntities>(nodeEntityId);

	//		for (ECS::Entity::Id childNodeId : childEntities->entitiesIds_) {
	//			StartNodeAnimation(world, childNodeId);
	//		}
	//	}
	//}

	void StartModelAnimation::Update(ModelEntity* modelEntity, StartAnimation* startAnimation) {

		const ECS2::Entity::Id rootNodeEntityId = modelEntity->id_;
		//StartNodeAnimation(world, rootNodeEntityId);

		//world->RemoveComponent<StartAnimation>(entityId);
	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> StartModelAnimation::GetFilter() const noexcept
	//{
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<ModelEntity>()
	//		.Include<StartAnimation>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId StartModelAnimation::GetTypeId() const noexcept {
	//	return Common::TypeInfo<StartModelAnimation>().GetId();
	//}



}