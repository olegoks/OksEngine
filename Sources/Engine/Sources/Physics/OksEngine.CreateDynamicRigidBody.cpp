
#include <Physics/auto_OksEngine.CreateDynamicRigidBody.hpp>

#include <glm/glm.hpp>
#include <PAL.RigidBody.hpp>

namespace OksEngine {

	void CreateDynamicRigidBody::Update(
		ECS2::Entity::Id entityId,
		const Position3D* position3D,
		const Material* material,
		const Mass* mass,
		const PhysicsShape* physicsShape,
		const DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape) {

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