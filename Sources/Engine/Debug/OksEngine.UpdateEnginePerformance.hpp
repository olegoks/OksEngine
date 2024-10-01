#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class UpdateEnginePerformance : public ECSSystem {
	public:

		UpdateEnginePerformance(Context& context);

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;

	};

}