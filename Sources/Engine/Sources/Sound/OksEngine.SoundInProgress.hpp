#pragma once 

#include <OksEngine.ECS.Component.hpp>
namespace OksEngine {

	struct SoundInProgress : public ECSComponent<SoundInProgress> {

		SoundInProgress() :
			ECSComponent{ nullptr } { }

	};

	template<>
	inline void Edit<SoundInProgress>(SoundInProgress* playSound) {

	}


}