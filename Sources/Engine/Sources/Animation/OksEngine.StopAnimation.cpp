
#include <Animation/OksEngine.StopAnimation.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>

#include <Animation/OksEngine.AnimationEnded.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.AnimationInProcess.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/Clock/auto_OksEngine.Clock.hpp>

namespace OksEngine {


	void StopAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		world->RemoveComponent<RunningAnimationState>(entityId);
		world->RemoveComponent<AnimationInProcess>(entityId);
		world->RemoveComponent<AnimationEnded>(entityId);
		world->RemoveComponent<Clock>(entityId);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> StopAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Animation>()
			.Include<RunningAnimationState>()
			.Include<AnimationInProcess>()
			.Include<AnimationEnded>()
			.Include<Clock>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId StopAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<StopAnimation>().GetId();
	}



}