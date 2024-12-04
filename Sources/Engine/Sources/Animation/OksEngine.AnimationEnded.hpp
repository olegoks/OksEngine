#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct AnimationEnded : public ECSComponent<AnimationEnded> {
		AnimationEnded() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<AnimationEnded>(AnimationEnded* animation) {

	}

}