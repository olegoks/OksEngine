
#include <Animation/OksEngine.ProcessAnimation.hpp>

#include <Animation/OksEngine.Animation.hpp>

#include <Animation/OksEngine.AnimationEnded.hpp>
#include <Animation/OksEngine.StartAnimation.hpp>

namespace OksEngine {


	void ProcessAnimation::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {



	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessAnimation::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Animation>()
			.Include<StartAnimation>()
			.Exclude<AnimationEnded>(), ECS::Entity::Filter{}.ExcludeAll()};
		return filter;
	}

	Common::TypeId ProcessAnimation::GetTypeId() const noexcept {
		return Common::TypeInfo<ProcessAnimation>().GetId();
	}



}