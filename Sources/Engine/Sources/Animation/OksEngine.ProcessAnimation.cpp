
#include <Animation/OksEngine.ProcessAnimation.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>

#include <Animation/OksEngine.AnimationEnded.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.AnimationInProcess.hpp>


#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Common/Clock/auto_OksEngine.Clock.hpp>

namespace OksEngine {


	void ProcessAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Position3D* position = world->GetComponent<Position3D>(entityId);
		Rotation3D* rotation = world->GetComponent<Rotation3D>(entityId);
		Animation* animation = world->GetComponent<Animation>(entityId);

		RunningAnimationState* animationState = world->GetComponent<RunningAnimationState>(entityId);
		Clock* clock = world->GetComponent<Clock>(entityId);

		std::chrono::nanoseconds animDuration{ (unsigned long long)(animation->durationInTicks_ / animation->ticksPerSecond_ * 1000000000) };
		std::chrono::high_resolution_clock::duration animationDuration = animDuration;
		//animation->ticksPerSecond_
		auto currentDelta = clock->value_ - animationState->start_;
		double currentTick = (double)currentDelta.count() / animationDuration.count() * animation->durationInTicks_;
		if (currentTick >= animation->durationInTicks_) {
			world->CreateComponent<AnimationEnded>(entityId);
			return;
		}

		//Process position.
		{
			Animation::PositionKey leftPositionKey;
			Animation::PositionKey rightPositionKey;

			for (Common::Index posKeyIndex = 0; posKeyIndex < animation->positionKeys_.size(); posKeyIndex++) {
				const Animation::PositionKey& positionKey = animation->positionKeys_[posKeyIndex];
				if (positionKey.time_ < currentTick) {
					leftPositionKey = positionKey;
					OS::AssertMessage(posKeyIndex + 1 < animation->positionKeys_.size(), "");
					rightPositionKey = animation->positionKeys_[posKeyIndex + 1];
				}
			}

			const double xDelta = rightPositionKey.value_.x - leftPositionKey.value_.x;
			const double yDelta = rightPositionKey.value_.y - leftPositionKey.value_.y;
			const double zDelta = rightPositionKey.value_.z - leftPositionKey.value_.z;

			double integerPart = 0.f;
			double fractionalPart = std::modf(currentTick, &integerPart);

			position->x_ = leftPositionKey.value_.x + xDelta * fractionalPart;
			position->y_ = leftPositionKey.value_.y + yDelta * fractionalPart;
			position->z_ = leftPositionKey.value_.z + zDelta * fractionalPart;
		}
		//Rotation process.

		Animation::RotationKey leftRotationKey;
		Animation::RotationKey rightRotationKey;

		for (Common::Index rotKeyIndex = 0; rotKeyIndex < animation->rotationKeys_.size(); rotKeyIndex++) {
			const Animation::RotationKey& rotationKey = animation->rotationKeys_[rotKeyIndex];
			if (rotationKey.time_ < currentTick) {
				leftRotationKey = rotationKey;
				OS::AssertMessage(rotKeyIndex + 1 < animation->rotationKeys_.size(), "");
				rightRotationKey = animation->rotationKeys_[rotKeyIndex + 1];
			}
		}

		//const double xDelta = rightPositionKey.value_.x - leftPositionKey.value_.x;
		//const double yDelta = rightPositionKey.value_.y - leftPositionKey.value_.y;
		//const double zDelta = rightPositionKey.value_.z - leftPositionKey.value_.z;

		//double integerPart = 0.f;
		//double fractionalPart = std::modf(currentTick, &integerPart);

		//position->xyz_.x = leftPositionKey.value_.x + xDelta * fractionalPart;
		//position->xyz_.y = leftPositionKey.value_.y + yDelta * fractionalPart;
		//position->xyz_.z = leftPositionKey.value_.z + zDelta * fractionalPart;


		rotation->w_ = rightRotationKey.value_.w;
		rotation->x_ = rightRotationKey.value_.x;
		rotation->y_ = rightRotationKey.value_.y;
		rotation->z_ = rightRotationKey.value_.z;
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Position3D>()
			.Include<Rotation3D>()
			.Include<Animation>()
			.Include<RunningAnimationState>()
			.Include<AnimationInProcess>()
			.Include<Clock>()
			.Exclude<AnimationEnded>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId ProcessAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<ProcessAnimation>().GetId();
	}



}