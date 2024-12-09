#pragma once

#include <OksEngine.ECS.Component.hpp>

#include <chrono>

namespace OksEngine {

	class Clock : public ECSComponent<Clock> {
	public:

		Clock() : ECSComponent{ nullptr } {

		}

		std::chrono::high_resolution_clock::time_point now_;
	};


	template<>
	inline void Edit<Clock>(Clock* clock) {
		ImGui::TextDisabled("Time since epoch(ns): %llu", clock->now_.time_since_epoch().count());
	}

}