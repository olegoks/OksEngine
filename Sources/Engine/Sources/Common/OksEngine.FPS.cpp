#pragma once
#include <Common\auto_OksEngine.FPS.hpp>

namespace OksEngine
{
	void CreateFPSSettingsEntity::Update() {
	
		ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<FPSLimit>(entityId, 1000);
		CreateComponent<CurrentFrameIndex>(entityId, 0);
	
	};

	void UpdateCurrentFrameIndex::Update(ECS2::Entity::Id entity0id, CurrentFrameIndex* currentFrameIndex0) {
		++currentFrameIndex0->value_;
	}

	void ExtendPreviousFrameDuration::Update(
		ECS2::Entity::Id entity0id, const Clock* clock0, const PreviousFrameDuration* previousFrameDuration0,
		const FrameEndTimePoint* frameEndTimePoint0, ECS2::Entity::Id entity1id, const FPSLimit* fPSLimit1) {
		
		const Common::UInt64 needFrameDuration = 1'000'000 / fPSLimit1->value_;
		if (needFrameDuration < previousFrameDuration0->microseconds_) {
			return;
		}
		const Common::UInt64 needWaitMicroS = needFrameDuration - previousFrameDuration0->microseconds_;

		std::chrono::microseconds duration(needWaitMicroS);

		std::this_thread::sleep_for(duration);

	};

} // namespace OksEngine