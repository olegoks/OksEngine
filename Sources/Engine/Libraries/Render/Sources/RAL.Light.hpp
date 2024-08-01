#pragma once

#include <Common.hpp>
#include <Math.Vector.hpp>

namespace RAL {

	class Light {
	public:

		struct CreateInfo {
			Common::Float intensity_ = 1.f;
			glm::vec3 position_ = { 0.f, 0.f, 0.f };
		};

		Light(const CreateInfo& createInfo) noexcept :
			intensity_{ createInfo.intensity_ },
			position_ { createInfo.position_} { }

		[[nodiscard]]
		const glm::vec3& GetPosition() const noexcept { return position_; }

		[[nodiscard]]
		Common::Float GetIntensity() const noexcept { return intensity_; }

	private:
		Common::Float intensity_ = 1.f;
		glm::vec3 position_ = { 0.f, 0.f, 0.f };
	};

}