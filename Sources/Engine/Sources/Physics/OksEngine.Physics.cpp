#pragma once
#include <Physics\auto_OksEngine.Physics.hpp>

namespace OksEngine
{
	namespace Physics
	{

		void EditShapeGeometryData(std::shared_ptr<ECS2::World> ecsWorld, ShapeGeometryData* shapeGeometryData) {



		}

		void AddShapeGeometryData(ECS2::World* ecsWorld, ECS2::Entity::Id entityId) {

		}

		ShapeGeometryData ParseShapeGeometryData(luabridge::LuaRef& shapeGeometryDataRef) {

			return ShapeGeometryData{};
		}

		std::string SerializeShapeGeometryData(const ShapeGeometryData* shapeGeometryData) {

			return "";
		}

		void CreateEngine::Update() {

			ECS2::Entity::Id physicsEngineEntity = CreateEntity();

			auto physicsEngine = std::make_shared<PE::PhysicsEngine>();
			CreateComponent<Engine>(physicsEngineEntity, physicsEngine);

		}

		void CreateWorld::Update(ECS2::Entity::Id entity0id, const Physics::Engine* engine0) {
		
			PAL::World::CreateInfo worldCreateInfo{
				.gravity_ = { 0.f, -9.81f, 0.f }
			};

			auto world = engine0->engine_->CreateWorld(worldCreateInfo);

			ECS2::Entity::Id physicsWorldEntity = CreateEntity();
			CreateComponent<World>(physicsWorldEntity, world);
			

		};

		void CreateStaticRigidBody::Update(
			ECS2::Entity::Id entity0id,
			World* world0,
			ECS2::Entity::Id entity1id, 
			const Name* name1,
			const Position3D* position3D1,
			const Rotation3D* rotation3D1,
			const Material* material1,
			const PhysicsShape* physicsShape1,
			const StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape1) {
		

			PAL::StaticRigidBody::CreateInfo srbCreateInfo{
				.rbCreateInfo_ = {
					.transform_ = glm::mat4{ glm::translate(glm::vec3(position3D1->x_, position3D1->y_, position3D1->z_)) } ,
					.shape_ = physicsShape1->shape_,
					.name_ = name1->value_
				}
			};

			const PAL::StaticRigidBody::Id srbId = world0->world_->CreateStaticRigidBody(srbCreateInfo);
			world0->world_->AddStaticRigidBody(srbId);

			CreateComponent<StaticRigidBody>(entity1id, srbId);

		
		};


		void CreatePhysicsShapeForStaticRigidBody::Update(ECS2::Entity::Id entity0id,
			const StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape0,
			ECS2::Entity::Id entity1id, Engine* physicsEngine1) {
		
		//
//		auto* materialComponent = world->GetComponent<Material>(entityId);
//		auto* modelEntity = world->GetComponent<ModelEntity>(entityId);
//
//		auto* meshsEntities = world->GetComponent<ChildEntities>(modelEntity->id_);
//
//		const ECS::Entity::Id meshEntityId = meshsEntities->entitiesIds_[0];
//
//		auto* vertices = world->GetComponent<Vertices3D>(meshEntityId);
//		auto* indices = world->GetComponent<Indices>(meshEntityId);
//
//		PAL::Shape::Material material{
//			.staticFriction_ = materialComponent->staticFriction_,
//			.dynamicFriction_ = materialComponent->dynamicFriction_,
//			.restitution_ = materialComponent->restitution_
//		};
//
//		PAL::Shape::CreateInfoMesh shapeCreateInfo{
//			.material_ = material,
//			.vertices_ = vertices->vertices_,
//			.indices_ = indices->indices_
//		};
//
//		auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);
//
//		world->CreateComponent<PhysicsShape>(entityId, shape);
//
//		//{
////	DynamicRigidBodyCustomMeshShape* dynamicRigidBody = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);
//
////	if (dynamicRigidBody != nullptr) {
////		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
////		if (dynamicRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
////			auto modelGeom = physicsSubsystem->GetGeom(dynamicRigidBody->meshName_);
////			PAL::Shape::CreateInfoMesh shapeCreateInfo{
////				.material_ = PAL::Shape::Material{
////					.staticFriction_ = dynamicRigidBody->material_.staticFriction_,
////					.dynamicFriction_ = dynamicRigidBody->material_.dynamicFriction_,
////					.restitution_ = dynamicRigidBody->material_.restitution_
////				},
////				.shape_ = *(modelGeom->begin())
////			};
//
////			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
////			PAL::RigidBody::CreateInfo createInfo{
////				.transform_ = dynamicRigidBody->transform_,
////				.shape_ = shape
////			};
////			dynamicRigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
////			physicsSubsystem->AddRigidBodyToWorld(dynamicRigidBody->id_);
////		}
//
////		//}
////		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
////		////Delete rotate component
////		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
////		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
////		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
////		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
////		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
////		//rigidBodyCapsule->SetTransform(rbTransform);
////	}
////}
		
		};


		void CreatePhysicsShapeForDynamicRigidBody::Update(
			ECS2::Entity::Id entity0id, 
			const Material* material0,
			const Vertices3D* vertices3D0,
			const Indices* indices0,
			const DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape0,
			
			ECS2::Entity::Id entity1id,
			Engine* physicsEngine1) {
		
		
//
//		auto* meshsEntities = world->GetComponent<ChildEntities>(modelEntity->id_);
//
//		const ECS::Entity::Id meshEntityId = meshsEntities->entitiesIds_[0];
//
//		auto* vertices = world->GetComponent<Vertices3D>(meshEntityId);
//		auto* indices = world->GetComponent<Indices>(meshEntityId);
//
//		PAL::Shape::Material shapeMaterial{
//			.staticFriction_ = material->staticFriction_,
//			.dynamicFriction_ = material->dynamicFriction_,
//			.restitution_ = material->restitution_
//		};
//
//		PAL::Shape::CreateInfoMesh shapeCreateInfo{
//			.material_ = material,
//			.vertices_ = vertices->vertices_,
//			.indices_ = indices->indices_
//		};
//
//		auto shape = GetContext().GetPhysicsSubsystem()->CreateShape(shapeCreateInfo);
//
//		world->CreateComponent<PhysicsShape>(entityId, shape);
//
//		//{
////	DynamicRigidBodyCustomMeshShape* dynamicRigidBody = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);
//
////	if (dynamicRigidBody != nullptr) {
////		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
////		if (dynamicRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
////			auto modelGeom = physicsSubsystem->GetGeom(dynamicRigidBody->meshName_);
////			PAL::Shape::CreateInfoMesh shapeCreateInfo{
////				.material_ = PAL::Shape::Material{
////					.staticFriction_ = dynamicRigidBody->material_.staticFriction_,
////					.dynamicFriction_ = dynamicRigidBody->material_.dynamicFriction_,
////					.restitution_ = dynamicRigidBody->material_.restitution_
////				},
////				.shape_ = *(modelGeom->begin())
////			};
//
////			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
////			PAL::RigidBody::CreateInfo createInfo{
////				.transform_ = dynamicRigidBody->transform_,
////				.shape_ = shape
////			};
////			dynamicRigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
////			physicsSubsystem->AddRigidBodyToWorld(dynamicRigidBody->id_);
////		}
//
////		//}
////		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
////		////Delete rotate component
////		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
////		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
////		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
////		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
////		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
////		//rigidBodyCapsule->SetTransform(rbTransform);
////	}
////}

		};

		void CreateDynamicRigidBody::Update(ECS2::Entity::Id entity0id, const Position3D* position3D0,
			const Material* material0, const Mass* mass0, const PhysicsShape* physicsShape0,
			const DynamicRigidBodyCustomMeshShape* dynamicRigidBodyCustomMeshShape0,
			ECS2::Entity::Id entity1id, Engine* physicsEngine1) {


		//PAL::DynamicRigidBody::CreateInfo createInfo{
		//	.rbCreateInfo_ = {
		//		.transform_ = glm::mat4{ glm::translate(glm::vec3( position3D->x_, position3D->y_, position3D->z_ )) },
		//		.shape_ = physicsShape->shape_,
		//		.name_ = "DynamicRigidBody"
		//	},
		//	.mass_ = mass->value_,
		//};
		//PAL::DRB::Id drbId = GetContext().GetPhysicsSubsystem()->GetWorld()->CreateDynamicRigidBody(createInfo);
		//GetContext().GetPhysicsSubsystem()->GetWorld()->AddDynamicRigidBody(drbId);

		//world->CreateComponent<DynamicRigidBody>(entityId, drbId);


			};

		void SimulatePhysics::Update(ECS2::Entity::Id entity0id, Engine* physicsEngine0) {
		
		
		using namespace std::chrono_literals;
	static std::chrono::high_resolution_clock::time_point previousUpdate = std::chrono::high_resolution_clock::now();
	static std::chrono::high_resolution_clock::duration remainder = 0ms;
	const auto simulationGranularity = 100ms;
	const auto now = std::chrono::high_resolution_clock::now();
	const auto delta = (now - previousUpdate);
	auto toSimulate = delta + remainder;

	while (toSimulate >= simulationGranularity) {
		physicsEngine0->engine_->GetWorld()->Simulate(simulationGranularity.count() / 1000.f);
		toSimulate -= simulationGranularity;
	}
	remainder = toSimulate;
	previousUpdate = std::chrono::high_resolution_clock::now();
		};

	}

} // namespace OksEngine