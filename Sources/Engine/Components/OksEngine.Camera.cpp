
#include <OksEngine.Camera.hpp>

namespace OksEngine {


	void Camera::DirectionRight(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(up_, direction_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));

	}


	void Camera::DirectionLeft(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(direction_, up_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));

	}

	void  Camera::DirectionUp(float angle) {
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), up_));
	}

	void  Camera::DirectionDown(float angle) {
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(-angle), up_));
	}


}