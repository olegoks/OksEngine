#pragma once

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class CollectECSSystemsCallsInfo : public ECSSystem {
	public:
	public:

		CollectECSSystemsCallsInfo(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "CollectECSSystemsCallsInfo";
		}
	private:
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	};


}