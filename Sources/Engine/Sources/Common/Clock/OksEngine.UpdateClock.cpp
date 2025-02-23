

#include <Common/Clock/auto_OksEngine.UpdateClock.hpp>

#include <chrono>

namespace OksEngine {

	void UpdateClock::Update(ECS2::Entity::Id entityId, const Clock* clock) {

		//clock->value_ = std::chrono::high_resolution_clock::now();
	}

}