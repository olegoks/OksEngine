#pragma once

#include <cmath>

namespace Math {

	constexpr float pi = 3.1415926535897932384626433832795028841971;

	template<class Type>
	Type Abs(Type value) noexcept {
		return std::abs(value);
	}

}