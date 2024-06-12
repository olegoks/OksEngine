#pragma once

#include <OksEngine.Subsystem.hpp>

#include <ECS.hpp>
#include <UI.Window.hpp>

namespace OksEngine {

	class UISubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};

		UISubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::UI, createInfo.context_ } {

			auto& context = GetContext();
			auto config = context.GetConfig();

			api_ = std::make_shared<UI::API>();
			UI::Window::CreateInfo windowCreateInfo;
			{
				windowCreateInfo.title_ = config->GetValueAs<std::string>("UI.Window.Title");
				windowCreateInfo.windowSize_ = {
					config->GetValueAs<int>("UI.Window.Width"),
					config->GetValueAs<int>("UI.Window.Height")
				};
			}
			window_ = api_->CreateWindow(windowCreateInfo);
			window_->Show();
			
		}

		std::shared_ptr<UI::Window> GetWindow() const noexcept {
			OS::AssertMessage(window_ != nullptr,
				"Attempt to get window pointer that was not initialized.");
			return window_;
		}

		virtual void Update() override {
			window_->ProcessInput();
		}

	private:

		std::shared_ptr<UI::API> api_ = nullptr;
		std::shared_ptr<UI::Window> window_ = nullptr;

	};

}