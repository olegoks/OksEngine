#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct Camera : public ECSComponent<Camera> {
		Math::Vector3f position_ = { 5.f, 0.f, 0.f };;
		Math::Vector3f direction_ = Math::Vector3f{ Math::Vector3f{ 0.f, 0.f, 0.f } - Math::Vector3f{ 5.f, 0.f, 0.f } };
		bool isActive_ = true;

		float GetDirectionX() {
			return direction_.GetX();
		}
		float GetDirectionY() {
			return direction_.GetY();
		}
		float GetDirectionZ() {
			return direction_.GetZ();
		}

		void SetDirection(float x, float y, float z) {
			direction_ = { x, y, z};
		}

		void SetDirectionX(float x) {
			direction_.GetX() = x;
		}
		void SetDirectionY(float y) {
			direction_.GetY() = y;
		}
		void SetDirectionZ(float z) {
			direction_.GetZ() = z;
		}

		Camera(
			Context* context,
			const Math::Vector3f& position,
			const Math::Vector3f& direction) : ECSComponent{ context } { }
		
	};

}