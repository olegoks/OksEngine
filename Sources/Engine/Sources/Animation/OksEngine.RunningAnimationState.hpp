#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <chrono>

namespace OksEngine {


	struct RunningAnimationState : public ECSComponent<RunningAnimationState> {
		RunningAnimationState() : ECSComponent{ nullptr } {}
		std::chrono::high_resolution_clock::time_point start_;
	};

	template<>
	inline void Edit<RunningAnimationState>(RunningAnimationState* animation) {

	}

}