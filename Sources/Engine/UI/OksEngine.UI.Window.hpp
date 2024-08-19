#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>

#include <UI/OksEngine.UI.KeyboardInput.hpp>
#include <UI/OksEngine.UI.Window.hpp>

namespace OksEngine {

	struct Window : public ECSComponent<Window> {
		Window(std::shared_ptr<UIAL::Window> window) noexcept : window_{ window }, ECSComponent { nullptr } { }
		std::string title_ = "";
		std::shared_ptr<UIAL::Window> window_ = nullptr;
	}; 



	template<>
	inline void Edit<Window>(Window* window) {

	}

	class GetWindowKeyboardInputEvents : public ECSSystem {
	public:
		GetWindowKeyboardInputEvents(Context& context) noexcept :
			ECSSystem{ context } { }


		virtual void Update(ECS::World* world, ECS::Entity::Id firstEntityId, ECS::Entity::Id secondEntityId) override {

			Window* window = world->GetComponent<Window>(firstEntityId);
			KeyboardInput* keyboardInput = world->GetComponent<KeyboardInput>(firstEntityId);
			OS::AssertMessage(keyboardInput->events_.empty(), "keyboard input in the frame beggining must be empty.");
			auto maybeEvent = window->window_->GetEvent();
			while (maybeEvent.has_value()) {
				keyboardInput->PushEvent({ maybeEvent.value().key_, maybeEvent.value().event_ } );
				maybeEvent = window->window_->GetEvent();
			}
		}

		Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<GetWindowKeyboardInputEvents>().GetId();
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Window>().Include<KeyboardInput>(), ECS::Entity::Filter{}.ExcludeAll()};
		}

	};

	class CleanWindowKeyboardInputEvents : public ECSSystem {
	public:
		CleanWindowKeyboardInputEvents(Context& context) noexcept :
			ECSSystem{ context } { }


		virtual void Update(ECS::World* world, ECS::Entity::Id firstEntityId, ECS::Entity::Id secondEntityId) override {

			Window* window = world->GetComponent<Window>(firstEntityId);
			KeyboardInput* keyboardInput = world->GetComponent<KeyboardInput>(firstEntityId);
			while (!keyboardInput->events_.empty()) keyboardInput->events_.pop();
		}

		Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<CleanWindowKeyboardInputEvents>().GetId();
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Window>().Include<KeyboardInput>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

	};

	class SendWindowKeyboardInputEvents : public ECSSystem {
	public:
		SendWindowKeyboardInputEvents(Context& context) noexcept :
			ECSSystem{ context } { }


		virtual void Update(ECS::World* world, ECS::Entity::Id firstEntityId, ECS::Entity::Id secondEntityId) override {

			KeyboardInput* windowKeyboardInput = world->GetComponent<KeyboardInput>(firstEntityId);
			HandleKeyboardInputMarker* handleKeyboardInputMarker = world->GetComponent<HandleKeyboardInputMarker>(secondEntityId);
			if (!world->IsComponentExist<KeyboardInput>(secondEntityId)) {
				world->CreateComponent<KeyboardInput>(secondEntityId);
			}
			if (world->IsComponentExist<KeyboardInput>(secondEntityId)) {
				KeyboardInput* keyboardInput = world->GetComponent<KeyboardInput>(secondEntityId);
				windowKeyboardInput->ForEachEvents([keyboardInput](const KeyboardInput::Event& event) {
					keyboardInput->PushEvent(event);
					});
			}
		}

		Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<SendWindowKeyboardInputEvents>().GetId();
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Window>().Include<KeyboardInput>(), ECS::Entity::Filter{}.Include<HandleKeyboardInputMarker>()};
		}

	};

	class WindowSystem : public ECSSystem {
	public:

		WindowSystem(Context& context) noexcept :
			ECSSystem{ context } { }

	
		virtual void Update(ECS::World* world, ECS::Entity::Id firstEntityId, ECS::Entity::Id secondEntityId) override {

			Window* window = world->GetComponent<Window>(firstEntityId);
			KeyboardInput* keyboardInput = world->GetComponent<KeyboardInput>(firstEntityId);
			keyboardInput->ForEachEvents([window](const KeyboardInput::Event& event) {
				if (event.key_ == UIAL::Window::Key::F5 && event.event_ == UIAL::Window::Event::Released) {
					window->window_->EnableCursor();
				}
				});
		}

		Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<WindowSystem>().GetId();
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Window>().Include<KeyboardInput>(), ECS::Entity::Filter{}.ExcludeAll()};
		}

	};



}