#pragma once

#include <ECS.hpp>
#include <Math.Vector.hpp>
namespace OksEngine {

	struct Position : public ECS::IComponent<Position> {
		glm::vec3 translate_{ 0.f, 0.f, 0.f };
		glm::quat rotate_{ };
	
		Position() = default; 
		Position(float x, float y, float z) :
			translate_{ x, y, z },
			rotate_{  } {}
		Position(const glm::vec3& translate, const glm::quat& rotate) : 
			translate_{ translate },
			rotate_{ rotate } {}

		[[nodiscard]]
		float GetX() { return translate_.x; }
		[[nodiscard]]
		float GetY() { return translate_.y; }
		[[nodiscard]]
		float GetZ() { return translate_.z; }

		[[nodiscard]]
		const glm::vec3& GetTranslateVec() const noexcept { return translate_; }

		[[nodiscard]]
		const glm::quat& GetRotateQuat() const noexcept { return rotate_; }

		[[nodiscard]]
		const glm::mat4 GetTranslateMat() const noexcept { return glm::translate(translate_); }

		[[nodiscard]]
		const glm::mat4 GetRotateMat() const noexcept { return glm::toMat4(rotate_); }

		void SetX(float x) { translate_.x = x; }
		void SetY(float y) { translate_.y = y; }
		void SetZ(float z) { translate_.z = z; }
	};


}