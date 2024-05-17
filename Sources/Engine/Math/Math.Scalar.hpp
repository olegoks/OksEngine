#pragma once

#include <Common.hpp>
#include <Math.Common.hpp>

namespace Math {

	constexpr Common::Double epsilon = 0.0001;

	[[nodiscard]]
	bool IsEqual(Common::Float first, Common::Float second) noexcept {
		return Abs<Common::Float>(first - second) < epsilon;
	}

}