#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <PAL.Shape.hpp>

namespace OksEngine {

	class PhysicsShape : public ECSComponent<PhysicsShape> {
	public:

		PhysicsShape() : ECSComponent{ nullptr } {}

		PhysicsShape(std::shared_ptr<PAL::Shape> shape) :
			ECSComponent{ nullptr },
			shape_{ shape } {
			
		}

		std::shared_ptr<PAL::Shape> shape_ = nullptr;

	};

	class CreatePhysicsShape : public ECSSystem {
	public:

		CreatePhysicsShape(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;


		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	private:
	};

}