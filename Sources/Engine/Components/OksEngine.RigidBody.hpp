#pragma once 
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.Physics.Subsystem.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	struct Material {
		float staticFriction_;
		float dynamicFriction_;
		float restitution_;
	};

	template<class RigidBodyType>
	struct RigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		float mass_ = 10.f;
		float linearDamping_ = 1.f;
		float angularDamping_ = 1.f;
		Material material_{ 0.5, 0.5f, 0.5f };

		RigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

		RigidBody(
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


	class RigidBodyBox : public RigidBody<RigidBodyBox> {
	public:

		RigidBodyBox() : RigidBody{ } {}

		RigidBodyBox(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			const Material& material,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ) :
			RigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				material
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
			const Material& material,
			float radius,
			float height) :
			RigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				material
			},
			radius_{ radius },
			height_{ height } {}

		float radius_ = 0.1f;
		float height_ = 0.5f;
	};

	class RigidBodyCustomMeshShape : public RigidBody<RigidBodyCustomMeshShape> {
	public:

		RigidBodyCustomMeshShape() : RigidBody{ } {}

		RigidBodyCustomMeshShape(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			const Material& material) :
			RigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				material
			}{}
	};



	template<class RigidBodyType>
	struct StaticRigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		//float mass_ = 10.f;
		//float linearDamping_ = 1.f;
		//float angularDamping_ = 1.f;
		Material material_{ 0.5, 0.5f, 0.5f };

		StaticRigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

		StaticRigidBody(
			Context* context,
			const glm::mat4& transform,
			//float mass,
			//float linearDamping,
			//float angularDamping,
			const Material& material
		) :
			ECSComponent<RigidBodyType>{ context },
			transform_{ transform },
			//mass_{ mass },
			//linearDamping_{ linearDamping },
			//angularDamping_{ angularDamping },
			material_{ material } { }

		//void SetTransform(const glm::mat4& transform) {
		//	transform_ = transform;
		//}

		const glm::mat4& GetTransform() {
			return transform_;
		}

	};

	class StaticRigidBodyCustomMeshShape : public StaticRigidBody<StaticRigidBodyCustomMeshShape> {
	public:

		StaticRigidBodyCustomMeshShape() : StaticRigidBody{ } {}

		StaticRigidBodyCustomMeshShape(
			Context* context,
			const glm::mat4& transform,
			const std::string& geomName,
			//float mass,
			//float linearDamping,
			//float angularDamping,
			const Material& material) :
			StaticRigidBody{
				context,
				transform,
				//mass,
				//linearDamping,
				//angularDamping,
				material
			},   
			geomName_{ geomName } {}
		std::string geomName_ = "";
	};




}