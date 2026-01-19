#pragma once 

#include <PAL.Constraint.hpp>

#include <PxPhysicsAPI.h>

namespace PhysX {

	class Constraint {
	public:

		void SetBreakForce(float force) {
			ASSERT(joint_ != nullptr);
			joint_->setBreakForce(force, 1000.0);
		}

		void SetInvMassScale0(float invMassScale) {
			ASSERT(joint_ != nullptr);
			joint_->setInvMassScale0(invMassScale);
		}

		void SetInvMassScale1(float invMassScale) {
			ASSERT(joint_ != nullptr);
			joint_->setInvMassScale1(invMassScale);
		}

	protected:

		void SetPtr(physx::PxJoint* joint) {
			ASSERT(joint_ == nullptr);
			joint_ = joint;
		}


	private:
		physx::PxJoint* joint_ = nullptr;
	};

	class FixedConstraint : public PAL::FixedConstraint, public Constraint {
	public:
		struct CreateInfo {
			physx::PxPhysics* physics_ = nullptr;
			physx::PxRigidActor* first_ = nullptr;
			physx::PxTransform	firstTr_;
			physx::PxRigidActor* second_ = nullptr;
			physx::PxTransform secondTr_;
		};

		using CI = CreateInfo;

		FixedConstraint(const CreateInfo& ci) 
			: PAL::FixedConstraint{}{
			
			physx::PxFixedJoint* fixedJoint = physx::PxFixedJointCreate(*ci.physics_, ci.first_, ci.firstTr_, ci.second_, ci.secondTr_);

			fixedJoint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
			// ¬ключить улучшенную стабильность (использовать улучшенный алгоритм решени€)
			//fixedJoint->setConstraintFlag(physx::PxConstraintFlag::eIMPROVED_SLERP, true);

			SetPtr(fixedJoint);

		}

	};

	class RevoluteConstraint : public PAL::RevoluteConstraint, public Constraint {
	public:
		struct CreateInfo {
			physx::PxPhysics* physics_ = nullptr;
			physx::PxRigidActor* first_ = nullptr;
			physx::PxTransform	firstTr_;
			physx::PxRigidActor* second_ = nullptr;
			physx::PxTransform secondTr_;
		};

		RevoluteConstraint(const CreateInfo& ci) 
			: PAL::RevoluteConstraint{} {

			physx::PxRevoluteJoint* revoluteJoint = physx::PxRevoluteJointCreate(*ci.physics_, ci.first_, ci.firstTr_, ci.second_, ci.secondTr_);

			revoluteJoint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

			

			SetPtr(revoluteJoint);
		}
	};

}