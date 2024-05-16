#include <Engine.hpp>

namespace OksEngine {

	Engine::Engine() noexcept {

		world_.RegisterSystem<BehaviourSystem>();
		//world_.RegisterSystem<RenderSystem>();

	}

}