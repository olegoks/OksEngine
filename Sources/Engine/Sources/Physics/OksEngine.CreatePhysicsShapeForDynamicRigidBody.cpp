#include <Physics\auto_OksEngine.CreatePhysicsShapeForDynamicRigidBody.hpp>

namespace OksEngine {
void Update(
    ECS2::Entity::Id entity1Id, const Material* material,
    const Vertices3D* vertices3D, const Indices* indices,
    const ImmutableRenderGeometry* immutableRenderGeometry,
    const ModelEntity* modelEntity,
    const DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape,
    ECS2::Entity::Id entity2Id, PhysicsEngine* physicsEngine) {};
}