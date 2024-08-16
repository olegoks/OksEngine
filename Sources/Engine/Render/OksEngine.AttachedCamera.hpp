#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct AttachedCamera : public ECSComponent<AttachedCamera> {
		
		glm::mat4 offset_{ 1.f };
	};

	template<>
	inline void Edit<AttachedCamera>(AttachedCamera* camera) {

	}

}