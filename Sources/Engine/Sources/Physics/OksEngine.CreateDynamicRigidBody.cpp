
#include <Physics/OksEngine.CreateDynamicRigidBody.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCustomMeshShape.hpp>
#include <Physics/OksEngine.Mass.hpp>
#include <Physics/OksEngine.Material.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>

namespace OksEngine {

	void CreateDynamicRigidBody::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position3D>(entityId);
		auto* shape = world->GetComponent<PhysicsShape>(entityId);
		auto* mass = world->GetComponent<Mass>(entityId);
		auto* material = world->GetComponent<Material>(entityId);

		PAL::DynamicRigidBody::CreateInfo createInfo{
			.rbCreateInfo_ = {
				.transform_ = glm::mat4{ glm::translate(glm::vec3( position->x_, position->y_, position->z_ )) },
				.shape_ = shape->shape_,
				.name_ = "DynamicRigidBody"
			},
			.mass_ = mass->mass_,
		};
		PAL::DRB::Id drbId = GetContext().GetPhysicsSubsystem()->GetWorld()->CreateDynamicRigidBody(createInfo);
		GetContext().GetPhysicsSubsystem()->GetWorld()->AddDynamicRigidBody(drbId);

		world->CreateComponent<DynamicRigidBody>(entityId, drbId);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDynamicRigidBody::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Position3D>()
			.Include<Material>()
			.Include<Mass>()
			.Include<PhysicsShape>()
			.Include<DynamicRigidBodyCustomMeshShape>()
			.Exclude<DynamicRigidBody>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateDynamicRigidBody::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDynamicRigidBody>().GetId();
	}


}