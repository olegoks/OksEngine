#include <Engine.hpp>

namespace OksEngine {

	Engine::Engine() noexcept {

		uiSubsystem_ = std::make_shared<UISubsystem>();
		world_.RegisterSystem<BehaviourSystem>();

	}

}