
#include <PAL.World.hpp>

namespace PAL {


	[[nodiscard]]
	RigidBody::Id PAL::World::GenerateId(std::shared_ptr<RigidBody> rbPtr) {
		RigidBody::Id rbId = rbIdGenerator_.GenerateECSCXXFilesStructure();
		IdRb_[rbId] = rbPtr;
		return rbId;
	}

	std::shared_ptr<RigidBody> PAL::World::GetRigidBodyById(DynamicRigidBody::Id drbid) {
		return IdRb_[drbid];
	}


}