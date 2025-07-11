#pragma once
#include <Common\Clock\auto_OksEngine.Clock.hpp>

#include <chrono>

namespace OksEngine
{
	void CreateClockEntity::Update() {
		
		const ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<Name>(entityId, "Clock");

		CreateComponent<TimeSinceEngineStart>(entityId, 0);

		const auto nowTimePoint = std::chrono::high_resolution_clock::now();
		const auto timeSinceEpoch = nowTimePoint.time_since_epoch();
		const auto microsecondsSinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(timeSinceEpoch);
		
		CreateComponent<TimeSinceEpoch>(entityId, microsecondsSinceEpoch.count());

		CreateComponent<EngineStartTimePoint>(entityId, nowTimePoint);
	}

	void UpdateTimeSinceEpoch::Update(ECS2::Entity::Id entity0id, TimeSinceEpoch* timeSinceEpoch0) {

		const auto timeSinceEpoch = std::chrono::high_resolution_clock::now().time_since_epoch();
		const auto microsecondsSinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(timeSinceEpoch);
		timeSinceEpoch0->microseconds_ = microsecondsSinceEpoch.count();

	}

	void UpdateTimeSinceEngineStart::Update(
		ECS2::Entity::Id entity0id,
		TimeSinceEngineStart* timeSinceEngineStart0,
		const EngineStartTimePoint* engineStartTimePoint0) {

		const auto durationSinceEngineStart = std::chrono::high_resolution_clock::now() - engineStartTimePoint0->timepoint_;
		const auto microsecondsSinceEngineStart = std::chrono::duration_cast<std::chrono::microseconds>(durationSinceEngineStart);

		timeSinceEngineStart0->microseconds_ = microsecondsSinceEngineStart.count();

	}

}