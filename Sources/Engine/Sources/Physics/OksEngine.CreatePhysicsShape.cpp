
#include <Physics/OksEngine.CreatePhysicsShape.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/Model/OksEngine.ModelEntity.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCustomMeshShape.hpp>
#include <Physics/OksEngine.StaticRigidBodyCustomMeshShape.hpp>
#include <Physics/OksEngine.Material.hpp>
#include <Render/Model/OksEngine.Mesh.hpp>
#include <Common/OksEngine.ChildEntity.hpp>

namespace OksEngine {


	void CreatePhysicsShapeForDynamicRigidBody::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* drbComponent = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);
		auto* materialComponent = world->GetComponent<Material>(entityId);
		auto* modelEntity = world->GetComponent<ModelEntity>(entityId);
		
		auto* meshsEntities = world->GetComponent<ChildEntities>(modelEntity->id_);

		const ECS::Entity::Id meshEntityId = meshsEntities->entitiesIds_[0];

		auto* vertices = world->GetComponent<Vertices>(meshEntityId);
		auto* indices = world->GetComponent<Indices>(meshEntityId);

		PAL::Shape::Material material{
			.staticFriction_ = materialComponent->staticFriction_,
			.dynamicFriction_ = materialComponent->dynamicFriction_,
			.restitution_ = materialComponent->restitution_
		};

		PAL::Shape::CreateInfoMesh shapeCreateInfo{
			.material_ = material,
			.vertices_ = vertices->vertices_,
			.indices_ = indices->indices_
		};

		auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);

		world->CreateComponent<PhysicsShape>(entityId, shape);

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

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreatePhysicsShapeForDynamicRigidBody::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Include<ModelEntity>()
			.Include<DynamicRigidBodyCustomMeshShape>()
			.Exclude<PhysicsShape>()
			.Exclude<DynamicRigidBody>(), ECS::Entity::Filter{}.ExcludeAll()};
	}

	Common::TypeId CreatePhysicsShapeForDynamicRigidBody::GetTypeId() const noexcept {
		return Common::TypeInfo<CreatePhysicsShapeForDynamicRigidBody>().GetId();
	}


	void CreatePhysicsShapeForStaticRigidBody::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* materialComponent = world->GetComponent<Material>(entityId);
		auto* modelEntity = world->GetComponent<ModelEntity>(entityId);

		auto* meshsEntities = world->GetComponent<ChildEntities>(modelEntity->id_);

		const ECS::Entity::Id meshEntityId = meshsEntities->entitiesIds_[0];

		auto* vertices = world->GetComponent<Vertices>(meshEntityId);
		auto* indices = world->GetComponent<Indices>(meshEntityId);

		PAL::Shape::Material material{
			.staticFriction_ = materialComponent->staticFriction_,
			.dynamicFriction_ = materialComponent->dynamicFriction_,
			.restitution_ = materialComponent->restitution_
		};

		PAL::Shape::CreateInfoMesh shapeCreateInfo{
			.material_ = material,
			.vertices_ = vertices->vertices_,
			.indices_ = indices->indices_
		};

		auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);

		world->CreateComponent<PhysicsShape>(entityId, shape);

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

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreatePhysicsShapeForStaticRigidBody::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Include<ModelEntity>()
			.Include<StaticRigidBodyCustomMeshShape>()
			.Exclude<PhysicsShape>()
			.Exclude<StaticRigidBody>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreatePhysicsShapeForStaticRigidBody::GetTypeId() const noexcept {
		return Common::TypeInfo<CreatePhysicsShapeForStaticRigidBody>().GetId();
	}


}