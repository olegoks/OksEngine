#pragma once

#include <memory>

#include <OksEngine.Config.hpp>
#include <OksEngine.CommandLineParameters.hpp>

namespace OksEngine {

	struct Context {
	public:

		struct CreateInfo {
			CommandLineParameters commandLineParameters_;
		};

		Context(const CreateInfo& createInfo);
		std::shared_ptr<class LogSubsystem> logSubsystem_ = nullptr;
		std::shared_ptr<class AsyncResourceSubsystem> resourceSubsystem_ = nullptr;
		std::shared_ptr<class RenderSubsystem> renderSubsystem_ = nullptr;
		std::shared_ptr<class UISubsystem> uiSubsystem_ = nullptr;
		std::shared_ptr<class ECS::World> world_;
		Config config_;
	private:

	};


}