#pragma once

#include <Common.Types.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	template<Common::UInt64 size, class BaseType>
	using Vector = Math::Vector<size, BaseType>;
	using Length = Common::Float;
	using Color3f = Math::Vector3f;
	using Normal3f = Math::Vector3f;
	using UV2f = Math::Vector2f;

}