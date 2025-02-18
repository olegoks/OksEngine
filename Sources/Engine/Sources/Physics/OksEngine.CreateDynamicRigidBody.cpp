
#include <Physics/auto_OksEngine.CreateDynamicRigidBody.hpp>

#include <glm/glm.hpp>
#include <PAL.RigidBody.hpp>

namespace OksEngine {

	void CreateDynamicRigidBody::Update(
		Position3D* position3D,
		Material* material, 
		Mass* mass, 
		PhysicsShape* physicsShape,
		DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape) {

		//PAL::DynamicRigidBody::CreateInfo createInfo{
		//	.rbCreateInfo_ = {
		//		.transform_ = glm::mat4{ glm::translate(glm::vec3( position3D->x_, position3D->y_, position3D->z_ )) },
		//		.shape_ = physicsShape->shape_,
		//		.name_ = "DynamicRigidBody"
		//	},
		//	.mass_ = mass->value_,
		//};
		//PAL::DRB::Id drbId = GetContext().GetPhysicsSubsystem()->GetWorld()->CreateDynamicRigidBody(createInfo);
		//GetContext().GetPhysicsSubsystem()->GetWorld()->AddDynamicRigidBody(drbId);

		//world->CreateComponent<DynamicRigidBody>(entityId, drbId);

	}

}