#pragma once

#include <Common.hpp>
#include <Math.Vector.hpp>

namespace RAL {

	class Light {
	public:

		struct CreateInfo {
			Common::Float intensity_ = 1.f;
			Math::Vector3f position_ = { 0.f, 0.f, 0.f };
		};

		Light(const CreateInfo& createInfo) noexcept :
			intensity_{ createInfo.intensity_ },
			position_ { createInfo.position_} { }

		[[nodiscard]]
		const Math::Vector3f& GetPosition() const noexcept { return position_; }

		[[nodiscard]]
		Common::Float GetIntensity() const noexcept { return intensity_; }

	private:
		Common::Float intensity_ = 1.f;
		Math::Vector3f position_ = { 0.f, 0.f, 0.f };
	};

}