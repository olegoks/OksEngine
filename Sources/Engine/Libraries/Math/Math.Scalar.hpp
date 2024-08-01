#pragma once

#include <Common.hpp>
#include <Math.Common.hpp>

namespace Math {

	[[nodiscard]]
	inline bool IsEqual(Common::Float first, Common::Float second) noexcept {
		constexpr static Common::Double epsilon = 0.0001;
		return Abs<Common::Float>(first - second) < epsilon;
	}

}