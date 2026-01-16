#pragma once 
#include <PAL.World.hpp>
#include <PxScene.h>
#include <physx/characterkinematic/PxControllerManager.h>

namespace PhysX {

	class World : public PAL::World {
	public:

		struct CreateInfo {
			PAL::World::CreateInfo palWorldCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		World(const CreateInfo& createInfo);

		virtual PAL::DynamicRigidBody::Id CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo) override;
		virtual PAL::StaticRigidBody::Id CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo) override;

		[[nodiscard]]
		physx::PxRigidActor* GetRigidBodyActor(PAL::RigidBody::Id rbId);
		

		virtual PAL::Constraint::Id CreateFixedConstraint(const PAL::FixedConstraint::CI& ci) override;
		virtual PAL::Constraint::Id CreateRevoluteConstraint(const PAL::RevoluteConstraint::CI& ci) override;

		[[nodiscard]]
		virtual PAL::CapsuleController::Id CreateCapsuleController(const PAL::CapsuleController::CreateInfo& srbCreateInfo) override;

		virtual void AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) override;
		virtual void ApplyForce(PAL::DynamicRigidBody::Id drbId) override;
		virtual Math::Vector3f GetRigidBodyPosition(PAL::DynamicRigidBody::Id drbId) override;
		virtual glm::quat GetRigidBodyRotation(PAL::DynamicRigidBody::Id drbId) override;

		virtual void AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) override;

		virtual void Simulate(float ms) override;
		PAL::CapsuleController::Id GenerateCapsuleControllerId(std::shared_ptr<physx::PxController> controller) {
			PAL::CapsuleController::Id rbId = capsuleControllerIdGenerator_.Generate();
			IdCapsuleController_[rbId] = controller;
			return rbId;
		}
	private:
		physx::PxScene* scene_ = nullptr;
		physx::PxPhysics* physics_ = nullptr;


		physx::PxControllerManager* controllerManager_ = nullptr;
		Common::IdGenerator capsuleControllerIdGenerator_;
		std::map<PAL::CapsuleController::Id, std::shared_ptr<physx::PxController>> IdCapsuleController_;
	};

}