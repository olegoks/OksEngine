#include <Physics/OksEngine.PhysicsShape.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCustomMeshShape.hpp>


namespace OksEngine {

	void CreatePhysicsShape::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		const auto* meshComponent = world->GetComponent<Mesh>(entityId);
		auto* drbComponent = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);
		auto* materialComponent = world->GetComponent<Material>(entityId);
		ModelStorage::Model& model = GetContext().GetModelStorage()->GetModel(meshComponent->modelId_);

		ModelStorage::Model::Mesh shapeModelMesh;

		model.ForEachMesh([&shapeModelMesh](ModelStorage::Model::Mesh& mesh) {

			shapeModelMesh = mesh;

				return false;
			});

		//Geom::Mesh& shapeMesh = GetContext().GetMeshStorage()->Get(shapeModelMesh.meshId_);

		//PAL::Shape::Material material{
		//	.staticFriction_ = materialComponent->staticFriction_,
		//	.dynamicFriction_ = materialComponent->dynamicFriction_,
		//	.restitution_ = materialComponent->restitution_
		//};

		//PAL::Shape::CreateInfoMesh shapeCreateInfo{
		//	.material_ = material,
		//	.vertices_ = shapeMesh.vertices_,
		//	.indices_ = shapeMesh.indices_
		//};

		//auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);

		//world->CreateComponent<PhysicsShape>(entityId, shape);

		//{
//	DynamicRigidBodyCustomMeshShape* dynamicRigidBody = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);

//	if (dynamicRigidBody != nullptr) {
//		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
//		if (dynamicRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
//			auto modelGeom = physicsSubsystem->GetGeom(dynamicRigidBody->meshName_);
//			PAL::Shape::CreateInfoMesh shapeCreateInfo{
//				.material_ = PAL::Shape::Material{
//					.staticFriction_ = dynamicRigidBody->material_.staticFriction_,
//					.dynamicFriction_ = dynamicRigidBody->material_.dynamicFriction_,
//					.restitution_ = dynamicRigidBody->material_.restitution_
//				},
//				.shape_ = *(modelGeom->begin())
//			};

//			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
//			PAL::RigidBody::CreateInfo createInfo{
//				.transform_ = dynamicRigidBody->transform_,
//				.shape_ = shape
//			};
//			dynamicRigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
//			physicsSubsystem->AddRigidBodyToWorld(dynamicRigidBody->id_);
//		}

//		//}
//		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
//		////Delete rotate component
//		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
//		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
//		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
//		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
//		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
//		//rigidBodyCapsule->SetTransform(rbTransform);
//	}
//}

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreatePhysicsShape::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Include<Mesh>()
			.Include<DynamicRigidBodyCustomMeshShape>(), ECS::Entity::Filter{} };
	}

	Common::TypeId CreatePhysicsShape::GetTypeId() const noexcept {
		return Common::TypeInfo<CreatePhysicsShape>().GetId();
	}

}
