#pragma once

#include <OS.Assert.hpp>
#include <Common.Types.hpp>

#include <Math.Vector.hpp>
#include <Math.Matrix.hpp>

namespace RAL {

	class [[nodiscard]] Camera {
	public:

		struct CreateInfo {
			glm::vec3 position_{ 0.f, 0.f, 0.f };
			glm::vec3 direction_{ 1.f, 0.f, 0.f };
			glm::vec3 up_{ 0.f, 0.f, 1.f };
			float zNear_ = 0.1f;
			float zFar_ = 1000000.f;
			glm::u32vec2 size_{ 0, 0 };
		};

		Camera(const CreateInfo& createInfo) noexcept :
			position_{ createInfo.position_ },
			direction_{ createInfo.direction_ },
			up_{ createInfo.up_ },
			zNear_{ createInfo.zNear_ },
			zFar_{ createInfo.zFar_ },
			width_{ createInfo.size_.x },
			height_{ createInfo.size_.y } { }

		const glm::vec3& GetDirection() const noexcept {
			return direction_;
		}

		const glm::vec3& GetPosition() const noexcept {
			return position_;
		}

		[[nodiscard]]
		const glm::vec3& GetUp() const noexcept {
			return up_;
		}

		[[nodiscard]]
		void SetUp(const glm::vec3& up) noexcept {
			up_ = up;
		}

		Common::Size GetWidth() const noexcept {
			return width_;
		}

		Common::Size GetHeight() const noexcept {
			return height_;
		}

		[[nodiscard]]
		float GetNearPlane() {
			return zNear_;
		}

		[[nodiscard]]
		float GetFarPlane() {
			return zFar_;
		}

		void SetPosition(const glm::vec3& position) {
			position_ = position;
		}

		void SetDirection(const glm::vec3& direction) {
			direction_ = direction;
		}

	private:
		glm::vec3 position_{ 0.f, 0.f, 0.f };
		glm::vec3 direction_{ 1.f, 0.f, 0.f };
		glm::vec3 up_{ 0.f, 0.f, 1.f };
		float zNear_ = 0.1f;
		float zFar_ = 100000.f;
		Common::Size width_ = 1920;
		Common::Size height_ = 1080;
	};

}