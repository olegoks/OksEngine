#pragma once 
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.Context.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	struct Material {
		float staticFriction_;
		float dynamicFriction_;
		float restitution_;
	};

	template<class RigidBodyType>
	struct DynamicRigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		float mass_ = 10.f;
		float linearDamping_ = 1.f;
		float angularDamping_ = 1.f;
		Material material_{ 0.5, 0.5f, 0.5f };

		DynamicRigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

		DynamicRigidBody(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			const Material& material
			) :
			ECSComponent<RigidBodyType>{ context },
			transform_{ transform },
			mass_{ mass },
			linearDamping_{ linearDamping },
			angularDamping_{ angularDamping },
			material_{ material } { }

		void SetTransform(const glm::mat4& transform) {
			transform_ = transform;
		}

		void ApplyForce(float x, float y, float z, float force) {
			this->GetContext().GetPhysicsSubsystem()->ApplyForce(id_, { x, y, z }, force);
		}

		void SetVelocity(float dirX, float dirY, float dirZ, float velocity) {
			if(id_ != Common::Limits<Common::Index>::Max())
			this->GetContext().GetPhysicsSubsystem()->SetVelocity(id_, { dirX, dirY, dirZ }, velocity);
		}

		const glm::mat4& GetTransform() {
			return transform_;
		}

	};




}