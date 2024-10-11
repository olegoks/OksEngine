#pragma once 

#include <Math.hpp>

namespace OksEngine {
	
	struct ViewProjection {
		alignas(16) glm::mat4 view_;
		alignas(16) glm::mat4 proj_;
	};

}