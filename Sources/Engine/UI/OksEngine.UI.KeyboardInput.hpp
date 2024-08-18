#pragma once 

#include <queue>

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct HandleKeyboardInputMarker : public ECSComponent<HandleKeyboardInputMarker> {
	public:
		HandleKeyboardInputMarker() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<HandleKeyboardInputMarker>(HandleKeyboardInputMarker* handleInputMarker) {

	}

	struct KeyboardInput : public ECSComponent<KeyboardInput> {
	public:
		KeyboardInput() : ECSComponent{ nullptr } {}

		struct Event {
			UI::Window::Key key_ = UI::Window::Key::Undefined;
			UI::Window::Event event_ = UI::Window::Event::Undefined;
		};

		std::queue<, >
	};

	template<>
	inline void Edit<KeyboardInput>(KeyboardInput* keyboardInput) {

	}

}