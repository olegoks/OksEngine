
#include <Animation/OksEngine.RunAnimation.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>

#include <Animation/OksEngine.AnimationEnded.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.AnimationInProcess.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Common/Clock/OksEngine.Clock.hpp>

namespace OksEngine {


	void RunAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Position* position = world->GetComponent<Position>(entityId);
		Animation* animation = world->GetComponent<Animation>(entityId);
		RunningAnimationState* animationState = world->GetComponent<RunningAnimationState>(entityId);
		Clock* clock = world->GetComponent<Clock>(entityId);
		animationState->start_ = clock->now_;
		world->RemoveComponent<StartAnimation>(entityId);
		world->CreateComponent<AnimationInProcess>(entityId);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RunAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Animation>()
			.Include<RunningAnimationState>()
			.Include<StartAnimation>()
			.Include<Clock>()
			.Exclude<AnimationEnded>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId RunAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<RunAnimation>().GetId();
	}



}