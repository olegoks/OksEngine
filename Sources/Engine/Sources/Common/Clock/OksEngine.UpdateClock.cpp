

#include <Common/Clock/OksEngine.UpdateClock.hpp>

#include <Common/Clock/auto_OksEngine.Clock.hpp>

#include <chrono>

namespace OksEngine {


	void UpdateClock::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* clock = world->GetComponent<Clock>(entityId);
		clock->value_ = std::chrono::high_resolution_clock::now();
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateClock::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
		{ ECS::Entity::Filter{}
			.Include<Clock>(),
			ECS::Entity::Filter{}.ExcludeAll() };

		return filter;

	}

}