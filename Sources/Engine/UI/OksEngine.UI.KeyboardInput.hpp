#pragma once 

#include <queue>

#include <OksEngine.ECS.Component.hpp>
#include <UI.Window.hpp>

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
			UIAL::Window::Key key_ = UIAL::Window::Key::Undefined;
			UIAL::Window::Event event_ = UIAL::Window::Event::Undefined;
		};

		std::queue<Event> events_;

		[[nodiscard]]
		Event GetEvent() noexcept {
			Event event = events_.front();
			events_.pop();
			return event;
		}

		void ForEachEvents(std::function<void(const Event& event)> eventProcessor) {
			std::queue<KeyboardInput::Event> eventsCopy = events_;
			while (!eventsCopy.empty()) {
				KeyboardInput::Event event = eventsCopy.front();
				eventsCopy.pop();
				eventProcessor(event);
			}
		}

		void PushEvent(const Event& event) noexcept {
			events_.push(event);
		}
	};

	template<>
	inline void Edit<KeyboardInput>(KeyboardInput* keyboardInput) {
		keyboardInput->ForEachEvents([](const KeyboardInput::Event& event) {
				const char* keyStr = magic_enum::enum_name<UIAL::Window::Key>(event.key_).data();
				OS::AssertMessage(keyStr != nullptr, "Error while getting enum name.");
				const char* eventStr = magic_enum::enum_name<UIAL::Window::Event>(event.event_).data();
				OS::AssertMessage(eventStr != nullptr, "Error while getting enum name.");
				ImGui::TextDisabled("Key: %s Event: %s", keyStr, eventStr);
			});
	}

}