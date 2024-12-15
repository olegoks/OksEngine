
#include <Animation/OksEngine.StartModelAnimation.hpp>

#include <Common/OksEngine.LocalPosition3D.hpp>

#include <Common/Clock/OksEngine.Clock.hpp>

#include <Common/OksEngine.ChildEntity.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>

#include <Render/Model/OksEngine.ModelEntity.hpp>

namespace OksEngine {

	void StartNodeAnimation(ECS::World* world, ECS::Entity::Id nodeEntityId) {

		if (world->IsComponentExist<Animation>(nodeEntityId)) {
			world->CreateComponent<StartAnimation>(nodeEntityId);
		}

		if (world->IsComponentExist<ChildEntities>(nodeEntityId)) {
			auto* childEntities = world->GetComponent<ChildEntities>(nodeEntityId);

			for (ECS::Entity::Id childNodeId : childEntities->entitiesIds_) {
				StartNodeAnimation(world, childNodeId);
			}
		}
	}

	void StartModelAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* model = world->GetComponent<ModelEntity>(entityId);

		const ECS::Entity::Id rootNodeEntityId = model->id_;
		StartNodeAnimation(world, rootNodeEntityId);

		world->RemoveComponent<StartAnimation>(entityId);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> StartModelAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<ModelEntity>()
			.Include<StartAnimation>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId StartModelAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<StartModelAnimation>().GetId();
	}



}