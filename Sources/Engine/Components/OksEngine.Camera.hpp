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

		float GetUpX() {
			return up_.x;
		}
		float GetUpY() {
			return up_.y;
		}
		float GetUpZ() {
			return up_.z;
		}


		void SetUpX(float x) {
			up_.x = x;
			
		}
		void SetUpY(float y) {
			up_.y = y;
		}
		void SetUpZ(float z) {
			up_.z = z;
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

			position_ = position_ + glm::normalize(up_) * delta;

		}

		void Down(float delta) {

			position_ = position_ - glm::normalize(up_) * delta;
		}

		void Left(float delta) {
			const glm::vec3 perpendicular = glm::normalize(glm::cross(up_, direction_));
			position_ = position_ + perpendicular * delta;
		}

		void Right(float delta) {
			const glm::vec3 perpendicular = glm::normalize(glm::cross(direction_, up_));
			position_ = position_ + perpendicular * delta;
		}

		void Transform(const glm::mat4& transform) {
			position_ = { transform[3][0], transform[3][1], transform[3][2] };
			//transform * glm::vec4{ position_, 1.f };
			//direction_ = transform * glm::vec4{ direction_, 1.f };
			//up_ = transform * glm::vec4{ up_, 1.f };
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