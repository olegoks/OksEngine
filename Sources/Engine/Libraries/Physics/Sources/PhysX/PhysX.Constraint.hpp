#pragma once 

#include <PAL.Constraint.hpp>

#include <PxPhysicsAPI.h>

namespace PhysX {

	class FixedConstraint : public PAL::FixedConstraint {
	public:
		struct CreateInfo {
			physx::PxPhysics* physics_ = nullptr;
			physx::PxRigidActor* first_ = nullptr;
			physx::PxTransform	firstTr_;
			physx::PxRigidActor* second_ = nullptr;
			physx::PxTransform secondTr_;
		};

		FixedConstraint(const CreateInfo& ci) : PAL::FixedConstraint{}{
			
			physx::PxFixedJoint* fixedJoint = physx::PxFixedJointCreate(*ci.physics_, ci.first_, ci.firstTr_, ci.second_, ci.secondTr_);

			fixedJoint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
			// ¬ключить улучшенную стабильность (использовать улучшенный алгоритм решени€)
			//fixedJoint->setConstraintFlag(physx::PxConstraintFlag::eIMPROVED_SLERP, true);

			ASSERT(joint_ == nullptr);
			joint_ = fixedJoint;

		}

		physx::PxFixedJoint* joint_ = nullptr;
	};


}