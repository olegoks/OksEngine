#pragma once

#include <Common/OksEngine.Subsystem.hpp>

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

		std::shared_ptr<class UI::Window> GetWindow() const noexcept;

		virtual void Update() override;
		
		std::shared_ptr<class UI::API> api_ = nullptr;
		std::shared_ptr<class UI::Window> window_ = nullptr;

	};

}