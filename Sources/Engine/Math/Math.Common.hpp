#pragma once

#include <cmath>

#define GLM_FORCE_RADIANS
//#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Math {

	constexpr float pi = 3.1415926535897932384626433832795028841971f;

	template<class Type>
	Type Abs(Type value) noexcept {
		return std::abs(value);
	}

}