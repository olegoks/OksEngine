
#include <Common/auto_OksEngine.RigidBodyToRenderGeometryMapper.hpp>

#include <Physics/OksEngine.Physics.Subsystem.hpp>
namespace OksEngine {

	void RigidBodyToRenderGeometryMapper::Update(
		ECS2::Entity::Id entityId,
		const Physics::DynamicRigidBody* dynamicRigidBody,
		const ImmutableRenderGeometry* immutableRenderGeometry) {
		//DynamicRigidBody* rigidBody = world->GetComponent<DynamicRigidBody>(entityId);
		//ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		//auto rigidBodyPtr = GetContext().GetPhysicsSubsystem()->GetWorld()->GetRigidBodyById(rigidBody->id_);

		////const auto& rigidBodyTransform = rigidBodyPtr->GetTransform();

	}

}