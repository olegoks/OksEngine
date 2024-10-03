#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class UpdateMainMenuBar : public ECSSystem {
	public:

		UpdateMainMenuBar(Context& context);

	public:

		virtual void BeforeUpdate(ECS::World* world) override;
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "UpdateMainMenuBar";
		}
	private:
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	};

}