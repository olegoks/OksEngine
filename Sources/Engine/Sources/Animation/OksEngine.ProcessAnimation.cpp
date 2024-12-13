
#include <Animation/OksEngine.ProcessAnimation.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>

#include <Animation/OksEngine.AnimationEnded.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.AnimationInProcess.hpp>

#include <Common/OksEngine.LocalPosition3D.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Common/OksEngine.Rotation.hpp>
#include <Common/Clock/OksEngine.Clock.hpp>

namespace OksEngine {


	void ProcessAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		LocalPosition3D* localPosition = world->GetComponent<LocalPosition3D>(entityId);
		Rotation* rotation = world->GetComponent<Rotation>(entityId);
		Animation* animation = world->GetComponent<Animation>(entityId);
		RunningAnimationState* animationState = world->GetComponent<RunningAnimationState>(entityId);
		Clock* clock = world->GetComponent<Clock>(entityId);
		std::chrono::nanoseconds animDuration{ (unsigned long long)(animation->duration_ / animation->ticksPerSecond_ * 1000000000) };
		std::chrono::high_resolution_clock::duration animationDuration = animDuration;
		//animation->ticksPerSecond_
		auto currentDelta = clock->now_ - animationState->start_;
		double currentTick = (double)currentDelta.count() / animationDuration.count() * animation->duration_;
		Common::Index leftStateIndex = std::floor(currentTick);
		Common::Index rightStateIndex = std::ceil(currentTick);
		if (rightStateIndex >= animation->states_.size() - 1) {
			world->CreateComponent<AnimationEnded>(entityId);
			return;
		}
		Animation::StateInfo& leftState = animation->states_[leftStateIndex];
		Animation::StateInfo& rightState = animation->states_[rightStateIndex];

		const double xDelta = rightState.translate_.x - leftState.translate_.x;
		const double yDelta = rightState.translate_.y - leftState.translate_.y;
		const double zDelta = rightState.translate_.z - leftState.translate_.z;

		double integerPart = 0.f;
		double fractionalPart = std::modf(currentTick, &integerPart);

		localPosition->xyz_.x = leftState.translate_.x + xDelta * fractionalPart;
		localPosition->xyz_.y = leftState.translate_.y + yDelta * fractionalPart;
		localPosition->xyz_.z = leftState.translate_.z + zDelta * fractionalPart;
	
		rotation->quat_.w = rightState.rotation_.w;
		rotation->quat_.x = rightState.rotation_.x;
		rotation->quat_.y = rightState.rotation_.y;
		rotation->quat_.z = rightState.rotation_.z;
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Rotation>()
			.Include<LocalPosition3D>()
			.Include<Animation>()
			.Include<RunningAnimationState>()
			.Include<AnimationInProcess>()
			.Include<Clock>()
			.Exclude<AnimationEnded>(), ECS::Entity::Filter{}.ExcludeAll()};
		return filter;
	}

	Common::TypeId ProcessAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<ProcessAnimation>().GetId();
	}



}