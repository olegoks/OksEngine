
#include <PAL.World.hpp>

namespace PAL {


	[[nodiscard]]
	RigidBody::Id PAL::World::GenerateRbId(std::shared_ptr<RigidBody> rbPtr) {
		RigidBody::Id rbId = rbIdGenerator_.Generate();
		IdRb_[rbId] = rbPtr;
		return rbId;
	}

	[[nodiscard]]
	Constraint::Id World::GenerateConstraintId(std::shared_ptr<Constraint> drbPtr) {
		Constraint::Id rbId = constraintIdGenerator_.Generate();
		IdConstraint_[rbId] = drbPtr;
		return rbId;
	}

	std::shared_ptr<RigidBody> PAL::World::GetRigidBodyById(DynamicRigidBody::Id drbid) {
		return IdRb_[drbid];
	}

	[[nodiscard]]
	std::shared_ptr<Constraint> World::GetConstraintById(Constraint::Id drbid) {
		return IdConstraint_[drbid];
	}


}