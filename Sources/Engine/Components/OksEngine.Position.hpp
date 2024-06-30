#pragma once

#include <ECS.hpp>
#include <Math.Vector.hpp>
namespace OksEngine {

	struct Position : public ECS::IComponent<Position> {
		Math::Vector3f xyz_{ 0.f, 0.f, 0.f };
		Position() = default;
		Position(float x, float y, float z) : xyz_{ x, y, z } {}
		[[nodiscard]]
		float GetX() { return xyz_.GetX(); }
		[[nodiscard]]
		float GetY() { return xyz_.GetY(); }
		[[nodiscard]]
		float GetZ() { return xyz_.GetZ(); }
		void SetX(float x) { xyz_.GetX() = x; }
		void SetY(float y) { xyz_.GetY() = y; }
		void SetZ(float z) { xyz_.GetZ() = z; }
	};


}