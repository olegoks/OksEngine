#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Width : public ECSComponent<Width> {
	public:

		Width() : ECSComponent{ nullptr } {}

		Width(Common::Size width) : ECSComponent{ nullptr }, value_{ width } {}

		Common::UInt64 value_ = 0;
	};


	template<>
	inline void Edit<Width>(Width* width) {
		ImGui::TextDisabled("Value: %d", width->value_);
	}

}