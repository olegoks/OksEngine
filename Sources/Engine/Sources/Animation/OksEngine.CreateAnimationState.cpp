
#include <Animation/OksEngine.CreateAnimationState.hpp>

#include <Common/OksEngine.LocalPosition3D.hpp>

#include <Common/Clock/OksEngine.Clock.hpp>

#include <Animation/OksEngine.Animation.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>
#include <Animation/OksEngine.RunningAnimationState.hpp>


namespace OksEngine {


	void CreateAnimationState::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		world->CreateComponent<RunningAnimationState>(entityId);
		world->CreateComponent<Clock>(entityId);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateAnimationState::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Animation>()
			.Include<StartAnimation>()
			.Exclude<RunningAnimationState>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateAnimationState::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateAnimationState>().GetId();
	}



}