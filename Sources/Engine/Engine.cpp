#include <Engine.hpp>

#include <Components/OksEngine.RenderableGeometry.hpp>


namespace OksEngine {

	Engine::Engine() noexcept {

		uiSubsystem_ = std::make_shared<UISubsystem>();
		world_.RegisterSystem<BehaviourSystem>();
		auto renderSystem = world_.RegisterSystem<RenderSystem>();

	}

}