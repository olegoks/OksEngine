#pragma once 

#include <queue>

#include <OksEngine.ECS.Component.hpp>
#include <UI/OksEngine.UIAL.Window.hpp>

namespace OksEngine {

	struct HandleMouseInputMarker : public ECSComponent<HandleMouseInputMarker> {
	public:
		HandleMouseInputMarker() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<HandleMouseInputMarker>(HandleMouseInputMarker* handleInputMarker) {

	}

	struct MouseInput : public ECSComponent<MouseInput> {
	public:
		MouseInput() : ECSComponent{ nullptr } {}
		using Event = UIAL::Window::MouseEvent;

		std::queue<Event> events_;

		[[nodiscard]]
		bool HasEvent() {
			return !events_.empty();
		}

		[[nodiscard]]
		Event GetEvent() noexcept {
			Event event = events_.front();
			events_.pop();
			return event;
		}

		void ForEachEvent(std::function<void(const Event& event)> eventProcessor) {
			std::queue<MouseInput::Event> eventsCopy = events_;
			while (!eventsCopy.empty()) {
				MouseInput::Event event = eventsCopy.front();
				eventsCopy.pop();
				eventProcessor(event);
			}
		}

		void PushEvent(const Event& event) noexcept {
			events_.push(event);
		}
	};

	template<>
	inline void Edit<MouseInput>(MouseInput* mouseInput) {
		mouseInput->ForEachEvent([](const MouseInput::Event& event) {
			const char* keyStr = magic_enum::enum_name<UIAL::Window::MouseKey>(event.key_).data();
			OS::AssertMessage(keyStr != nullptr, "Error while getting enum name.");
			const char* eventStr = magic_enum::enum_name<UIAL::Window::Event>(event.event_).data();
			OS::AssertMessage(eventStr != nullptr, "Error while getting enum name.");
			ImGui::TextDisabled("Key: %s Event: %s", keyStr, eventStr);
			});
	}

	template<>
	inline void Add<MouseInput>(ECS::World* world, ECS::Entity::Id id) { }

}