#pragma once

#include <ECS.hpp>
#include <Math.Vector.hpp>
#include <Math.Angle.hpp>

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Rotation : public ECSComponent<Rotation> {
		glm::quat quat_{ };

		Rotation() = default;
		Rotation(const glm::quat& quat) :
			ECSComponent{ nullptr },
			quat_{ quat } {}

		Rotation(const glm::vec3& rotationVector, Math::Angle angle) :
			ECSComponent{ nullptr },
			quat_{ glm::angleAxis(angle.ToRadians().GetValue(), glm::normalize(rotationVector)) } {
		
		}
		
		[[nodiscard]]
		glm::vec3 GetRotationVector() const noexcept {
			const glm::vec3 rotationVector = glm::axis(quat_);
			return glm::normalize(rotationVector);
		}

		[[nodiscard]]
		Math::Angle GetAngle() const noexcept {
			float angle = glm::angle(quat_);
			return Math::Angle{ angle };
		}

		[[nodiscard]]
		const glm::quat& GetQuat() const noexcept { return quat_; }

		[[nodiscard]]
		const glm::mat4 GetMat() const noexcept { return glm::toMat4(quat_); }

	};

	template<>
	inline void Edit<Rotation>(Rotation* rotation) {
	}


}
