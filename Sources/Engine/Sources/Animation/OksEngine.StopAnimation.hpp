#pragma once

#include <OksEngine.ECS.System.hpp>


namespace OksEngine {

	class StopAnimation : public ECSSystem {
	public:

		StopAnimation(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "StopAnimation";
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	private:
	};
}