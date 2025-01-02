
#include <Physics/OksEngine.CreateStaticRigidBody.hpp>

#include <Physics/OksEngine.StaticRigidBodyCustomMeshShape.hpp>
#include <Physics/OksEngine.StaticRigidBody.hpp>
#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Physics/OksEngine.Material.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Common/auto_OksEngine.Name.hpp>


namespace OksEngine {

	void CreateStaticRigidBody::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position3D>(entityId);
		auto* rotation = world->GetComponent<Rotation3D>(entityId);
		auto* shape = world->GetComponent<PhysicsShape>(entityId);
		auto* material = world->GetComponent<Material>(entityId);
		auto* name = world->GetComponent<Name>(entityId);

		PAL::StaticRigidBody::CreateInfo srbCreateInfo{
			.rbCreateInfo_ = {
				.transform_ = glm::mat4{ glm::translate(glm::vec3(position->x_, position->y_, position->z_)) } ,
				.shape_ = shape->shape_,
				.name_ = name->value_
			}
		};

		const PAL::StaticRigidBody::Id srbId = GetContext().GetPhysicsSubsystem()->GetWorld()->CreateStaticRigidBody(srbCreateInfo);
		GetContext().GetPhysicsSubsystem()->GetWorld()->AddStaticRigidBody(srbId);

		world->CreateComponent<StaticRigidBody>(entityId, srbId);


	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateStaticRigidBody::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}
			.Include<Position3D>()
			.Include<Rotation3D>()
			.Include<Name>()
			.Include<Material>()
			.Include<PhysicsShape>()
			.Include<StaticRigidBodyCustomMeshShape>()
			.Exclude<StaticRigidBody>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateStaticRigidBody::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateStaticRigidBody>().GetId();
	}


}