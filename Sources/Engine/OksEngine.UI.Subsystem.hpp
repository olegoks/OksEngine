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

	private:

		std::shared_ptr<UI::API> api_ = nullptr;
		std::shared_ptr<UI::Window> window_ = nullptr;

	};

}