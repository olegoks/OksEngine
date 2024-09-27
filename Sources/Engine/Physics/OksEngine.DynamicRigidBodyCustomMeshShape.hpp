#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	class DynamicRigidBodyCustomMeshShape : public ECSComponent<DynamicRigidBodyCustomMeshShape> {
	public:

		DynamicRigidBodyCustomMeshShape() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<DynamicRigidBodyCustomMeshShape>(DynamicRigidBodyCustomMeshShape* dynamicRigidBody) {

	}

	class Material : public ECSComponent<Material> {
	public:

		Material() : ECSComponent{ nullptr } {}

		Material(float staticFriction, float dynamicFriction, float restitution) : ECSComponent{ nullptr },
			staticFriction_{ staticFriction },
			dynamicFriction_{ dynamicFriction },
			restitution_{ restitution } {}

		float staticFriction_;
		float dynamicFriction_;
		float restitution_;
	};

	class Mass : public ECSComponent<Mass> {
	public:

		Mass() : ECSComponent{ nullptr } {}

		Mass(float mass) : ECSComponent{ nullptr }, mass_{ mass } {}

		float mass_ = 10.f;
	};

	//template<class RigidBodyType>
	//struct DynamicRigidBody : public ECSComponent<RigidBodyType> {
	//	Common::Index id_ = Common::Limits<Common::Index>::Max();
	//	float mass_ = 10.f;
	//	float linearDamping_ = 1.f;
	//	float angularDamping_ = 1.f;
	//	Material material_{ 0.5, 0.5f, 0.5f };

	//	DynamicRigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

	//	DynamicRigidBody(
	//		float mass,
	//		float linearDamping,
	//		float angularDamping,
	//		const Material& material
	//	) :
	//		ECSComponent<RigidBodyType>{ nullptr },
	//		mass_{ mass },
	//		linearDamping_{ linearDamping },
	//		angularDamping_{ angularDamping },
	//		material_{ material } { }

	//	void ApplyForce(float x, float y, float z, float force) {
	//		this->GetContext().GetPhysicsSubsystem()->ApplyForce(id_, { x, y, z }, force);
	//	}

	//	void SetVelocity(float dirX, float dirY, float dirZ, float velocity) {
	//		if (id_ != Common::Limits<Common::Index>::Max())
	//			this->GetContext().GetPhysicsSubsystem()->SetVelocity(id_, { dirX, dirY, dirZ }, velocity);
	//	}

	//};


	class DynamicRigidBody : public ECSComponent<DynamicRigidBody> {
	public:

		DynamicRigidBody() : ECSComponent{ nullptr } {}

		DynamicRigidBody(PAL::DynamicRigidBody::Id id) : ECSComponent{ nullptr }, id_ { id } {}

		PAL::DynamicRigidBody::Id id_;
	};

	class CreateDynamicRigidBody : public ECSSystem {
	public:

		CreateDynamicRigidBody(Context& context) : ECSSystem{ context } { }


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;


		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;

	private:
	};

}