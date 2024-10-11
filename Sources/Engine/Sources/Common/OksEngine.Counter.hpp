#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Counter : public ECSComponent<Counter> {
	public:

		Counter() : ECSComponent{ nullptr } {}

		Common::UInt64 value_ = 0;
	};


	template<>
	inline void Edit<Counter>(Counter* counter) {
		ImGui::TextDisabled("Value: %d", counter->value_);
	}

}