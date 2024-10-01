
#include <Common/OksEngine.RigidBodyToRenderGeometryMapper.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	RigidBodyToRenderGeometryMapper::RigidBodyToRenderGeometryMapper(Context& context) : ECSSystem{ context } {

	}


	void RigidBodyToRenderGeometryMapper::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		/*DynamicRigidBodyBox* rigidBody = world->GetComponent<DynamicRigidBodyBox>(entityId);
		if (rigidBody == nullptr) return;
		ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		if (renderGeometry == nullptr) return;
		const auto& rigidBodyTransform = rigidBody->GetTransform();
		renderGeometry->SetTransform(rigidBodyTransform);*/

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RigidBodyToRenderGeometryMapper::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}/*.Include<DynamicRigidBodyBox>()*/.Include<ImmutableRenderGeometry>(), ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId RigidBodyToRenderGeometryMapper::GetTypeId() const noexcept {
		return Common::TypeInfo<RigidBodyToRenderGeometryMapper>().GetId();
	}
}