#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {


	class AddECSInspectorMainMenuItem : public ECSSystem {
	public:

		AddECSInspectorMainMenuItem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "AddECSInspectorMainMenuItem";
		}
	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}