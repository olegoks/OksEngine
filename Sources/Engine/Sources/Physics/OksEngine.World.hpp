
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct World : public ECSComponent<World> {

		World(std::shared_ptr<World> world) : ECSComponent{ nullptr }, world_{ world } {}


		std::shared_ptr<World> world_ = nullptr;
	};

}