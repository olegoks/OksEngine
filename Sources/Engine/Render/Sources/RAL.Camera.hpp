#pragma once

#include <OS.Assert.hpp>
#include <Common.Types.hpp>

import OksEngine.OS.Assert;
import OksEngine.Common.Types;

import OksEngine.Math.Vector;
import OksEngine.Math.Matrix;

namespace RAL {

	class [[nodiscard]] Camera {
	public:

		struct CreateInfo {
			Math::Vector3f position_{ 0.f, 0.f, 0.f };
			Math::Vector3f direction_{ 1.f, 0.f, 0.f };
			float zNear_ = 0.1f;
			float zFar_ = 10.f;
			Common::Size width_ = 1920;
			Common::Size height_ = 1080;
		};

		Camera(const CreateInfo& createInfo) noexcept :
			position_{ createInfo.position_ },
			direction_{ createInfo.direction_ },
			zNear_{ createInfo.zNear_ },
			zFar_{ createInfo.zFar_ } { }

		const Math::Vector3f& GetDirection() const noexcept {
			return direction_;
		}

		const Math::Vector3f& GetPosition() const noexcept {
			return position_;
		}

		Common::Size GetWidth() const noexcept {
			return width_;
		}

		Common::Size GetHeight() const noexcept {
			return height_;
		}

		void SetPosition(const Math::Vector3f& position) {
			position_ = position;
			//UpdateMatrices();
		}

		void SetDirection(const Math::Vector3f& direction) {
			direction_ = direction;
			//UpdateMatrices();
		}


	private:

		//void UpdateMatrices() noexcept {
		//	view_ = Math::Matrix4x4f::GetView(GetPosition(), GetDirection(), { 0.f, 0.f, 1.f });
		//	projection_ = Math::Matrix4x4f::GetPerspective(45, width_ / (float)height_, zNear_, zFar_);
		//}

	private:
		//Math::Matrix4x4f view_;
		//Math::Matrix4x4f projection_;
		Math::Vector3f position_;
		Math::Vector3f direction_;
		float zNear_ = 0.1;
		float zFar_ = 10;
		Common::Size width_ = 1920;
		Common::Size height_ = 1080;
	};

}