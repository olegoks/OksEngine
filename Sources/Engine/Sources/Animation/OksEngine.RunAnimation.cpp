
#include <Animation/auto_OksEngine.RunAnimation.hpp>


namespace OksEngine {


	void RunAnimation::Update(
		ECS2::Entity::Id entityId,
		RunningAnimationState* runningAnimationState,
		const StartAnimation* startAnimation,
		const Clock* clock) {

		//runningAnimationState->start_ = clock->value_;
		//world->RemoveComponent<StartAnimation>(entityId);
		//world->CreateComponent<AnimationInProcess>(entityId);

	}


}