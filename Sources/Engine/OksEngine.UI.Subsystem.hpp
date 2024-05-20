#pragma once

#include <OksEngine.Subsystem.hpp>

#include <ECS.hpp>
#include <UI.hpp>

namespace OksEngine {

	class UISubsystem : public Subsystem {
	public:

		UISubsystem() {

			api_ = std::make_shared<UI::API>();
			UI::Window::CreateInfo windowCreateInfo;
			{
				windowCreateInfo.title_ = "OksEngine:Vulkan API, GLFW, IMGUI";
				windowCreateInfo.windowSize_ = { 1280, 600 };
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