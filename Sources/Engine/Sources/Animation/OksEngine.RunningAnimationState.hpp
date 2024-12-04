#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct RunningAnimationState : public ECSComponent<RunningAnimationState> {
		RunningAnimationState() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<RunningAnimationState>(RunningAnimationState* animation) {

	}

}