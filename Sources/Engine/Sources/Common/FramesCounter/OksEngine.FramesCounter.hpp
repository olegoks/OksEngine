#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class FramesCounter : public ECSComponent<FramesCounter> {
	public:

		FramesCounter() : ECSComponent{ nullptr } { }
	};

	template<>
	inline void Edit<FramesCounter>(FramesCounter* framesCounter) {	}

}