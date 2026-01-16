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