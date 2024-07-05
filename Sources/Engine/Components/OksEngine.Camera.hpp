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

		void DirectionUp() {

		}

		void Forward(float delta) {
			position_ = position_ + (glm::normalize(direction_) * delta);
		}

		void Backward(float delta){
			position_ = position_ - (glm::normalize(direction_) * delta);
		}

		void Up(float delta) {
			position_.y += delta;
		}

		void Down(float delta) {
			position_.y -= delta;
		}

		void Left(float delta) {
			//glm::vec3 dirProj{ direction_.GetX(), 0.0f, direction_.GetY() };
			//position_ = position_ + dirProj.CrossProduct(direction_).Normalize() * delta;
		}

		void Right(float delta) {
			//glm::vec3 dirProj{ direction_.GetX(), 0.0f, direction_.GetY() };
			//position_ = position_ - dirProj.CrossProduct(direction_).Normalize() * delta;
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