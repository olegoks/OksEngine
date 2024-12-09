#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct AnimationInProcess : public ECSComponent<AnimationInProcess> {
		AnimationInProcess() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<AnimationInProcess>(AnimationInProcess* animation) {

	}

}