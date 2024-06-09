#pragma once

#include <memory>

#include <OksEngine.Config.hpp>

namespace OksEngine {

	struct Context {
	public:
		Context(std::filesystem::path configFilePath);
		std::shared_ptr<class AsyncResourceSubsystem> resourceSubsystem_ = nullptr;
		std::shared_ptr<class RenderSubsystem> renderSubsystem_ = nullptr;
		std::shared_ptr<class UISubsystem> uiSubsystem_ = nullptr;
		std::shared_ptr<class ECS::World> world_;
		Config config_;
	private:

	};


}