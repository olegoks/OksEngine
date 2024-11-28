
#include <Common/OksEngine.RigidBodyToRenderGeometryMapper.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

#include <Physics/OksEngine.Physics.Subsystem.hpp>
namespace OksEngine {

	RigidBodyToRenderGeometryMapper::RigidBodyToRenderGeometryMapper(Context& context) : ECSSystem{ context } {

	}


	void RigidBodyToRenderGeometryMapper::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		DynamicRigidBody* rigidBody = world->GetComponent<DynamicRigidBody>(entityId);
		ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		auto rigidBodyPtr = GetContext().GetPhysicsSubsystem()->GetWorld()->GetRigidBodyById(rigidBody->id_);

		const auto& rigidBodyTransform = rigidBodyPtr->GetTransform();
		//renderGeometry->SetTransform(rigidBodyTransform);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RigidBodyToRenderGeometryMapper::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<DynamicRigidBody>().Include<ImmutableRenderGeometry>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId RigidBodyToRenderGeometryMapper::GetTypeId() const noexcept {
		return Common::TypeInfo<RigidBodyToRenderGeometryMapper>().GetId();
	}
}