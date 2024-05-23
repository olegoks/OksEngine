#pragma once

#include <memory>

namespace OksEngine {

	struct Context {
	public:
		Context();
		std::shared_ptr<class ResourceSubsystemAsyncInterface> resourceSubsystem_ = nullptr;
		std::shared_ptr<class RenderSubsystem> renderSubsystem_ = nullptr;
		std::shared_ptr<class UISubsystem> uiSubsystem_ = nullptr;
		std::shared_ptr<class ECS::World> world_;
	private:

	};


}