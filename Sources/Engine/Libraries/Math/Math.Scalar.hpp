#pragma once

#include <Common.hpp>
#include <Math.Common.hpp>

namespace Math {

	constexpr static Common::Double epsilon = 0.0001;

	[[nodiscard]]
	inline bool IsEqual(Common::Float first, Common::Float second) noexcept {
		return Abs<Common::Float>(first - second) < epsilon;
	}


	[[nodiscard]]
	inline bool IsLess(Common::Float first, Common::Float second) noexcept {
		return first < second + epsilon;
	}

}