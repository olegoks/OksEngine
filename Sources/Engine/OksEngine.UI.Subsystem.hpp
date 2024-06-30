#pragma once

#include <OksEngine.Subsystem.hpp>

#include <ECS.hpp>
#include <UI.Window.hpp>
#include <OksEngine.Config.hpp>

namespace OksEngine {

	class UISubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};

		UISubsystem(const CreateInfo& createInfo);

		std::shared_ptr<UI::Window> GetWindow() const noexcept;

		virtual void Update() override;
		

		struct Event {
			UI::Window::Key key_ = UI::Window::Key::Undefined;
			UI::Window::Event event_ = UI::Window::Event::Undefined;
		};

		std::optional<Event> GetLastEvent() {
			if (events_.empty()) {
				return {};
			}
			return events_.back();
		}

		void DeleteLastEvent() {
			if(!events_.empty())
				events_.pop_back();
		}

	private:

		void ProcessEvent(UI::Window::Key key, UI::Window::Event event);

	private:

		std::vector<Event> events_;
		std::shared_ptr<UI::API> api_ = nullptr;
		std::shared_ptr<UI::Window> window_ = nullptr;

	};

}