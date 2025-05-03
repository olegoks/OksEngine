#pragma once 

#include <queue>
//
//#include <OksEngine.ECS.Component.hpp>
#include <UI.Window.hpp>

namespace OksEngine {

	//struct HandleKeyboardInputMarker : public ECSComponent<HandleKeyboardInputMarker> {
	//public:
	//	HandleKeyboardInputMarker() : ECSComponent{ nullptr } {}

	//};

	//template<>
	//inline void Edit<HandleKeyboardInputMarker>(HandleKeyboardInputMarker* handleInputMarker) {

	//}

	/*struct KeyboardInput : public ECSComponent<KeyboardInput> {
	public:
		KeyboardInput() : ECSComponent{ nullptr } {}

		struct KeyboardAction {
			UIAL::Window::KeyboardKey key_ = UIAL::Window::KeyboardKey::Undefined;
			UIAL::Window::KeyboardAction event_ = UIAL::Window::KeyboardAction::Undefined;
		};

		std::queue<KeyboardAction> events_;

		[[nodiscard]]
		bool HasEvent() {
			return !events_.empty();
		}

		[[nodiscard]]
		KeyboardAction GetEvent() noexcept {
			KeyboardAction event = events_.front();
			events_.pop();
			return event;
		}

		void ForEachEvent(std::function<void(const KeyboardAction& event)> eventProcessor) {
			std::queue<KeyboardInput::KeyboardAction> eventsCopy = events_;
			while (!eventsCopy.empty()) {
				KeyboardInput::KeyboardAction event = eventsCopy.front();
				eventsCopy.pop();
				eventProcessor(event);
			}
		}

		void PushEvent(const KeyboardAction& event) noexcept {
			events_.push(event);
		}
	};

	template<>
	inline void Edit<KeyboardInput>(KeyboardInput* keyboardInput) {
		keyboardInput->ForEachEvent([](const KeyboardInput::KeyboardAction& event) {
				const char* keyStr = magic_enum::enum_name<UIAL::Window::KeyboardKey>(event.key_).data();
				OS::AssertMessage(keyStr != nullptr, "Error while getting enum name.");
				const char* eventStr = magic_enum::enum_name<UIAL::Window::KeyboardAction>(event.event_).data();
				OS::AssertMessage(eventStr != nullptr, "Error while getting enum name.");
				ImGui::TextDisabled("Key: %s Event: %s", keyStr, eventStr);
			});
	}

	template<>
	inline void Add<KeyboardInput>(ECS::World* world, ECS::Entity::Id id) { }
*/


	//struct FrameBufferResizeEvents : public ECSComponent<FrameBufferResizeEvents> {
	//public:
	//	FrameBufferResizeEvents() : ECSComponent{ nullptr } {}

	//	std::queue<UIAL::Window::FrameBufferResizeEvent> events_;

	//	[[nodiscard]]
	//	bool HasEvent() {
	//		return !events_.empty();
	//	}

	//	[[nodiscard]]
	//	UIAL::Window::FrameBufferResizeEvent GetEvent() noexcept {
	//		UIAL::Window::FrameBufferResizeEvent event = events_.front();
	//		events_.pop();
	//		return event;
	//	}

	//	void ForEachEvent(std::function<void(const UIAL::Window::FrameBufferResizeEvent& event)> eventProcessor) {
	//		std::queue<UIAL::Window::FrameBufferResizeEvent> eventsCopy = events_;
	//		while (!eventsCopy.empty()) {
	//			UIAL::Window::FrameBufferResizeEvent event = eventsCopy.front();
	//			eventsCopy.pop();
	//			eventProcessor(event);
	//		}
	//	}

	//	void PushEvent(const UIAL::Window::FrameBufferResizeEvent& event) noexcept {
	//		events_.push(event);
	//	}
	//};


	//class 


}