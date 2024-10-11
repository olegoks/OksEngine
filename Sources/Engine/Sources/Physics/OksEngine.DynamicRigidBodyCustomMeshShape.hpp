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


}