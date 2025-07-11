
#include <Animation/auto_OksEngine.StopAnimation.hpp>


namespace OksEngine {


	void StopAnimation::Update(
		ECS2::Entity::Id entityId,
		const Animation* animation,
		RunningAnimationState* runningAnimationState,
		const AnimationInProcess* animationInProcess,
		const AnimationEnded* animationEnded,
		const TimeSinceEpoch* clock) {

		//world->RemoveComponent<RunningAnimationState>(entityId);
		//world->RemoveComponent<AnimationInProcess>(entityId);
		//world->RemoveComponent<AnimationEnded>(entityId);
		//world->RemoveComponent<Clock>(entityId);

	}


}