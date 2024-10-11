#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Height : public ECSComponent<Height> {
	public:

		Height() : ECSComponent{ nullptr } {}

		Height(Common::Size value) : ECSComponent{ nullptr }, value_{ value } {}

		Common::UInt64 value_ = 0;
	};


	template<>
	inline void Edit<Height>(Height* height) {
		ImGui::TextDisabled("Value: %d", height->value_);
	}

}