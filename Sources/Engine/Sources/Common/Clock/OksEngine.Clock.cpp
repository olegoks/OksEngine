#pragma once
#include <Common\Clock\auto_OksEngine.Clock.hpp>

#include <chrono>

namespace OksEngine
{
	void CreateClockEntity::Update() {
		
		const ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<Name>(entityId, "Clock");
		CreateComponent<Clock>(entityId);
		CreateComponent<TimeSinceEngineStart>(entityId, 0);

		const auto nowTimePoint = std::chrono::high_resolution_clock::now();
		const auto timeSinceEpoch = nowTimePoint.time_since_epoch();
		const auto microsecondsSinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(timeSinceEpoch);
		
		CreateComponent<TimeSinceEpoch>(entityId, microsecondsSinceEpoch.count());

		CreateComponent<EngineStartTimePoint>(entityId, nowTimePoint);
		CreateComponent<FrameStartTimePoint>(entityId, nowTimePoint);
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

	void UpdateFrameStartTimePoint::Update(
		ECS2::Entity::Id entity0id,
		FrameStartTimePoint* frameStartTimePoint0) {

		const auto nowTimePoint = std::chrono::high_resolution_clock::now();
		frameStartTimePoint0->timepoint_ = nowTimePoint;

	}

	void UpdatePreviousFrameDuration::Update(
		ECS2::Entity::Id entity0id,
		PreviousFrameDuration* previousFrameDuration0,
		const FrameStartTimePoint* frameStartTimePoint0) {

		const auto durationSincePreviousFrameStart = std::chrono::high_resolution_clock::now() - frameStartTimePoint0->timepoint_;

		const auto microsecondsSincePreviousFrameStart = std::chrono::duration_cast<std::chrono::microseconds>(durationSincePreviousFrameStart);

		previousFrameDuration0->microseconds_ = microsecondsSincePreviousFrameStart.count();

	}

}