#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Text : public ECSComponent<Text> {

		Text(const std::string& text) : ECSComponent{ nullptr }, text_{ text } {}

		std::string text_;
	};

	template<>
	inline void Edit<Text>(Text* text) {

	}

}