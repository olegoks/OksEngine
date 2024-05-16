#include <Engine.hpp>

namespace OksEngine {

	Engine::Engine() noexcept {

		world_.RegisterSystem<BehaviourSystem>();
		//world_.RegisterSystem<RenderSystem>();

	}

	Engine::Engine(const Options& options) noexcept :
		options_{ options } {

		world_.RegisterSystem<BehaviourSystem>();

	}

}