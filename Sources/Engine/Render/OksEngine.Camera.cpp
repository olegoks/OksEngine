
#include <Render/OksEngine.Camera.hpp>

namespace OksEngine {


	void Camera::DirectionRight(float angle) {
		direction_ = glm::normalize(glm::rotate(
			direction_,
			glm::radians(-angle), 
			{ 0.f, 1.f, 0.f })
		);
		up_ = glm::normalize(glm::rotate(
			up_,
			glm::radians(-angle),
			{ 0.f, 1.f, 0.f })
		);
	}


	void Camera::DirectionLeft(float angle) {
		direction_ = glm::normalize(glm::rotate(
			direction_,
			glm::radians(angle),
			{ 0.f, 1.f, 0.f })
		);
		up_ = glm::normalize(glm::rotate(
			up_,
			glm::radians(angle),
			{ 0.f, 1.f, 0.f })
		);
	}

	void  Camera::DirectionUp(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(direction_, up_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));
	}

	void  Camera::DirectionDown(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(up_, direction_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));
	}


}