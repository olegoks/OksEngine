
#include <Physics/auto_OksEngine.CreateStaticRigidBody.hpp>

#include <PAL.RigidBody.hpp>
#include <glm/glm.hpp>

namespace OksEngine {

	void CreateStaticRigidBody::Update(
		ECS2::Entity::Id entity1Id,
		const Name* name,
		const Position3D* position3D,
		const Rotation3D* rotation3D,
		const Material* material,
		const PhysicsShape* physicsShape,
		const StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape,
		ECS2::Entity::Id entity2Id,
		PhysicsEngine* physicsEngine) {

		//PAL::StaticRigidBody::CreateInfo srbCreateInfo{
		//	.rbCreateInfo_ = {
		//		.transform_ = glm::mat4{ glm::translate(glm::vec3(position3D->x_, position3D->y_, position3D->z_)) } ,
		//		.shape_ = physicsShape->shape_,
		//		.name_ = name->value_
		//	}
		//};

		//const PAL::StaticRigidBody::Id srbId = GetContext().GetPhysicsSubsystem()->GetWorld()->CreateStaticRigidBody(srbCreateInfo);
		//GetContext().GetPhysicsSubsystem()->GetWorld()->AddStaticRigidBody(srbId);

		//world->CreateComponent<StaticRigidBody>(entityId, srbId);


	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateStaticRigidBody::GetFilter() const noexcept
	//{
	//	return { ECS::Entity::Filter{}
	//		.Include<Position3D>()
	//		.Include<Rotation3D>()
	//		.Include<Name>()
	//		.Include<Material>()
	//		.Include<PhysicsShape>()
	//		.Include<StaticRigidBodyCustomMeshShape>()
	//		.Exclude<StaticRigidBody>(), ECS::Entity::Filter{}.ExcludeAll() };
	//}


}