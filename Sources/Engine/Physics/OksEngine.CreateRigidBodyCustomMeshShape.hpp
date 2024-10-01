#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {



	class CreateDynamicRigidBody : public ECSSystem {
	public:

		CreateDynamicRigidBody(Context& context) : ECSSystem{ context } { }


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;


		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	private:
	};



}