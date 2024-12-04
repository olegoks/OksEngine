#pragma once

#include <OksEngine.ECS.System.hpp>

#include <Animation/OksEngine.RunningAnimationState.hpp>

namespace OksEngine {

	class CreateAnimationState : public ECSSystem {
	public:

		CreateAnimationState(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "CreateAnimationState";
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	private:
	};
}