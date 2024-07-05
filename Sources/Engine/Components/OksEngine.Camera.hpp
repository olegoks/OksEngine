#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct Camera : public ECSComponent<Camera> {
		glm::vec3 position_ = { 5.f, 0.f, 0.f };
		glm::vec3 direction_ = glm::vec3{ glm::vec3{ 0.f, 0.f, 0.f } - glm::vec3{ 5.f, 0.f, 0.f } };
		glm::vec3 up_ = { 0.f, -1.f, 0.f };
		bool isActive_ = true;

		float GetDirectionX() {
			return direction_.x;
		}
		float GetDirectionY() {
			return direction_.y;
		}
		float GetDirectionZ() {
			return direction_.z;
		}

		void SetDirection(float x, float y, float z) {
			direction_ = { x, y, z};
		}

		void SetDirectionX(float x) {
			direction_.x = x;
		}
		void SetDirectionY(float y) {
			direction_.y = y;
		}
		void SetDirectionZ(float z) {
			direction_.z = z;
		}

		void DirectionUp(float angle);
		void DirectionDown(float angle);
		void DirectionLeft(float angle);
		void DirectionRight(float angle);

		void Forward(float delta) {
			position_ = position_ + (glm::normalize(direction_) * delta);
		}

		void Backward(float delta){
			position_ = position_ - (glm::normalize(direction_) * delta);
		}

		void Up(float delta) {
			const glm::vec3 perpendicular = glm::normalize(glm::cross(up_, direction_));
			position_ = position_ + perpendicular * delta;
		}

		void Down(float delta) {
			const glm::vec3 perpendicular = glm::normalize(glm::cross(direction_, up_));
			position_ = position_ + perpendicular * delta;
		}

		void Left(float delta) {
			position_ = position_ + glm::normalize(up_) * delta;
		}

		void Right(float delta) {
			position_ = position_ - glm::normalize(up_) * delta;
		}

		Camera(
			Context* context,
			const glm::vec3& position,
			const glm::vec3& direction,
			const glm::vec3& up) :
			ECSComponent{ context },
			position_{ position },
			direction_{ direction },
			up_{ up } { }
		
	};

}