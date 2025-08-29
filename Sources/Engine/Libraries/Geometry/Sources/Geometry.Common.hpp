#pragma once

#include <Common.Types.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	template<Common::UInt64 size, class BaseType>
	using Vector = Math::Vector<size, BaseType>;
	using Vector3f = Math::Vector3f;
	using Length = Common::Float;
	using Color3f = Math::Vector3f;
	using Color4b = Math::Vector<4, Common::Byte>;
	using Color4f = Math::Vector4f;
	using Normal3f = Math::Vector3f;
	using UV2f = Math::Vector2f;
	using BonesIds = Math::Vector4ub;
	using Weights = Math::Vector4ub;
}