#pragma once 
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.Physics.Subsystem.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	template<class RigidBodyType>
	struct RigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		float mass_ = 10.f;
		float linearDamping_ = 1.f;
		float angularDamping_ = 1.f;
		float staticFriction_ = 0.5f;
		float dynamicFriction_ = 0.5f;
		float restitution_ = 0.5f;

		RigidBody() : ECSComponent<RigidBodyType>{ nullptr }
		{

		}
		RigidBody(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution) :
			ECSComponent<RigidBodyType>{ context },
			transform_{ transform },
			mass_{ mass },
			linearDamping_{ linearDamping },
			angularDamping_{ angularDamping },
			staticFriction_{ staticFriction },
			dynamicFriction_{ dynamicFriction },
			restitution_{ restitution } {	}

		void SetTransform(const glm::mat4& transform) {
			transform_ = transform;
		}

		void ApplyForce(float x, float y, float z, float force) {
			this->GetContext().GetPhysicsSubsystem()->ApplyForce(id_, { x, y, z }, force);
		}

		const glm::mat4& GetTransform() {
			return transform_;
		}

	};


	class RigidBodyBox : public RigidBody<RigidBodyBox> {
	public:

		RigidBodyBox() : RigidBody{ } {}

		RigidBodyBox(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ) :
			RigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				staticFriction,
				dynamicFriction,
				restitution
			},
			halfExtentX_{ halfExtentX },
			halfExtentY_{ halfExtentY },
			halfExtentZ_{ halfExtentZ } {}

		float halfExtentX_ = 0.5f;
		float halfExtentY_ = 0.5f;
		float halfExtentZ_ = 0.5f;
	};

	class RigidBodyCapsule : public RigidBody<RigidBodyCapsule> {
	public:

		RigidBodyCapsule() : RigidBody{ } {}

		RigidBodyCapsule(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float radius,
			float height) :
			RigidBody{
				context,
				transform,
				mass,
				linearDamping,
			    angularDamping,
				staticFriction,
				dynamicFriction,
				restitution,
			},
			radius_{ radius },
			height_{ height } {}

		float radius_ = 0.1f;
		float height_ = 0.5f;
	};

}