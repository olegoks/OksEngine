#pragma once

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {


	class CallInputProcessor : public ECSSystem {
	public:

		CallInputProcessor(Context& context) noexcept;

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual void EndUpdate() override { }
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "CallInputProcessor";
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	};

}