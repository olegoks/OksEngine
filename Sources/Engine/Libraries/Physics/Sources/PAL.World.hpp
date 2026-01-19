#pragma once 

#include <Math.Vector.hpp>

#include <PAL.RigidBody.hpp>
#include <PAL.Constraint.hpp>


namespace PAL {

	class CapsuleController {
	public:
		using Id = Common::Id;

		struct CreateInfo {
			glm::vec3 position_{};
			float capsuleHeight_ = 2.0f;
			float capsuleRadius_ = 0.5f;
		};

	};

	class World {
	public:
		struct CreateInfo {
			Math::Vector3f gravity_{ 0.f, -9.81, 0.f };
		};

		World(const CreateInfo& createInfo) : 
			createInfo_{ createInfo } { }

		[[nodiscard]]
		virtual PAL::DynamicRigidBody::Id CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo) = 0;
		[[nodiscard]]
		virtual PAL::StaticRigidBody::Id CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo) = 0;

		virtual PAL::Constraint::Id CreateFixedConstraint(
			const FixedConstraint::CI& ci) = 0;
		virtual PAL::Constraint::Id CreateRevoluteConstraint(const PAL::RevoluteConstraint::CI& ci) = 0;

		[[nodiscard]]
		virtual PAL::CapsuleController::Id CreateCapsuleController(const PAL::CapsuleController::CreateInfo& srbCreateInfo) = 0;

		[[nodiscard]]
		std::shared_ptr<RigidBody> GetRigidBodyById(RigidBody::Id drbid);

		[[nodiscard]]
		std::shared_ptr<Constraint> GetConstraintById(Constraint::Id drbid);

		virtual void ApplyForce(PAL::DynamicRigidBody::Id drbId) = 0;

		virtual Math::Vector3f GetRigidBodyPosition(PAL::DynamicRigidBody::Id drbId) = 0;
		virtual glm::quat GetRigidBodyRotation(PAL::DynamicRigidBody::Id drbId) = 0;

		virtual void AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) = 0;

		virtual void AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) = 0;

		virtual void Simulate(float ms) = 0;

		enum class DebugRenderParameters {

			eENABLE,
			eWORLD_AXES,
		
	/* Body visualizations */

		/**
		\brief Visualize a bodies axes.

		\see PxActor.globalPose PxActor
		*/
		eBODY_AXES,
		
		/**
		\brief Visualize a body's mass axes.

		This visualization is also useful for visualizing the sleep state of bodies. Sleeping bodies are drawn in
		black, while awake bodies are drawn in white. If the body is sleeping and part of a sleeping group, it is
		drawn in red.

		\see PxBodyDesc.massLocalPose PxActor
		*/
		eBODY_MASS_AXES,
		
		/**
		\brief Visualize the bodies linear velocity.

		\see PxBodyDesc.linearVelocity PxActor
		*/
		eBODY_LIN_VELOCITY,
		
		/**
		\brief Visualize the bodies angular velocity.

		\see PxBodyDesc.angularVelocity PxActor
		*/
		eBODY_ANG_VELOCITY,

	/* Contact visualisations */

		/**
		\brief  Visualize contact points. Will enable contact information.
		*/
		eCONTACT_POINT,
		
		/**
		\brief Visualize contact normals. Will enable contact information.
		*/
		eCONTACT_NORMAL,
		
		/**
		\brief  Visualize contact errors. Will enable contact information.
		*/
		eCONTACT_ERROR,
		
		/**
		\brief Visualize Contact impulses. Will enable contact information.
		*/
		eCONTACT_IMPULSE,

		/**
		\brief  Visualize friction points. Will enable contact information.
		*/
		eFRICTION_POINT,

		/**
		\brief Visualize friction normals. Will enable contact information.
		*/
		eFRICTION_NORMAL,

		/**
		\brief Visualize friction impulses. Will enable contact information.
		*/
		eFRICTION_IMPULSE,

		/**
		\brief Visualize actor axes.

		\see PxRigidStatic PxRigidDynamic PxArticulationLink
		*/
		eACTOR_AXES,

		/**
		\brief Visualize bounds (AABBs in world space)
		*/
		eCOLLISION_AABBS,
		
		/**
		\brief Shape visualization

		\see PxShape
		*/
		eCOLLISION_SHAPES,
		
		/**
		\brief Shape axis visualization

		\see PxShape
		*/
		eCOLLISION_AXES,

		/**
		\brief Compound visualization (compound AABBs in world space)
		*/
		eCOLLISION_COMPOUNDS,

		/**
		\brief Mesh & convex face normals

		\see PxTriangleMesh PxConvexMesh
		*/
		eCOLLISION_FNORMALS,
		
		/**
		\brief Active edges for meshes

		\see PxTriangleMesh
		*/
		eCOLLISION_EDGES,

		/**
		\brief Static pruning structures
		*/
		eCOLLISION_STATIC,

		/**
		\brief Dynamic pruning structures
		*/
		eCOLLISION_DYNAMIC,

		/**
		\brief Joint local axes
		*/
		eJOINT_LOCAL_FRAMES,

		/** 
		\brief Joint limits
		*/
		eJOINT_LIMITS,
	
		/**
		\brief Visualize culling box
		*/
		eCULL_BOX
		};

		virtual void SetDebugRenderParameters(DebugRenderParameters parameter, bool value) noexcept = 0;

		virtual void GetDebugRenderBufferPoints() const noexcept = 0;
		virtual Geom::VertexCloud<Geom::Vertex3fc> GetDebugRenderBufferTriangles() const noexcept = 0;
		virtual Geom::VertexCloud<Geom::Vertex3fc> GetDebugRenderBufferLines() const noexcept = 0;

	protected:

		[[nodiscard]]
		RigidBody::Id GenerateRbId(std::shared_ptr<RigidBody> drbPtr);

		[[nodiscard]]
		Constraint::Id GenerateConstraintId(std::shared_ptr<PAL::Constraint> drbPtr);

	private:
		CreateInfo createInfo_;
		Common::IdGenerator rbIdGenerator_;
		std::map<PAL::RigidBody::Id, std::shared_ptr<PAL::RigidBody>> IdRb_;

		Common::IdGenerator constraintIdGenerator_;
		std::map<PAL::Constraint::Id, std::shared_ptr<PAL::Constraint>> IdConstraint_;


	};

}