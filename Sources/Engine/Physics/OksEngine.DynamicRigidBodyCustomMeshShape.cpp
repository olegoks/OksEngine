#include "OksEngine.DynamicRigidBodyCustomMeshShape.hpp"

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>
#include <Common/OksEngine.Position.hpp>

namespace OksEngine {

	void CreateDynamicRigidBody::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position>(entityId);
		auto* shape = world->GetComponent<PhysicsShape>(entityId);
		auto* mass = world->GetComponent<Mass>(entityId);
		auto* material = world->GetComponent<Material>(entityId);

		PAL::DynamicRigidBody::CreateInfo createInfo{
			.transform_ = position->GetMat(),
			.mass_ = mass->mass_,
			.shape_ = shape->shape_
		};
		Common::Index rbId = GetContext().GetPhysicsSubsystem()->CreateRigidBody(createInfo);
		GetContext().GetPhysicsSubsystem()->AddRigidBodyToWorld(rbId);

		world->CreateComponent<DynamicRigidBody>(entityId, rbId);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDynamicRigidBody::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}
			.Include<Position>()
			.Include<Material>()
			.Include<Mass>()
			.Include<PhysicsShape>()
			.Include<DynamicRigidBodyCustomMeshShape>(), ECS::Entity::Filter{} };
	}

	Common::TypeId CreateDynamicRigidBody::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDynamicRigidBody>().GetId();
	}

}