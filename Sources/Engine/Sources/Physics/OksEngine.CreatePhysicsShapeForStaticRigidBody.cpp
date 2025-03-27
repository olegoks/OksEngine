#include <Physics\auto_OksEngine.CreatePhysicsShapeForStaticRigidBody.hpp>

namespace OksEngine {
void Update(
    ECS2::Entity::Id entity1Id,
    const ImmutableRenderGeometry* immutableRenderGeometry,
    const ModelEntity* modelEntity,
    const StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape,
    ECS2::Entity::Id entity2Id, PhysicsEngine* physicsEngine) {};
}