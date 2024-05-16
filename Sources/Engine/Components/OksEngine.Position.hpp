#pragma once

#include <ECS.hpp>

namespace OksEngine {

	struct Position : public ECS::IComponent<Position> {
		int x_ = 0;
		int y_ = 0;
		int z_ = 0;
		Position() = default;
		Position(int x, int y, int z) {}
		int GetX() { return x_; }
		int GetY() { return y_; }
		int GetZ() { return z_; }
		void SetX(int x) { x_ = x; }
		void SetY(int y) { y_ = y; }
		void SetZ(int z) { z_ = z; }
	};


}