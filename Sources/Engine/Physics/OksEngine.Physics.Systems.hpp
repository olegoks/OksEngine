#pragma once

#include <Physics/OksEngine.CreateRigidBodyCustomMeshShape.hpp>

namespace OksEngine {

	//class PhysicsSystem : public ECSSystem {
	//public:

	//	PhysicsSystem(Context& context) : ECSSystem{ context } {

	//	}


	//	virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
	//		//{
	//		//	DynamicRigidBodyBox* rigidBody = world->GetComponent<DynamicRigidBodyBox>(entityId);

	//		//	if (rigidBody != nullptr) {
	//		//		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
	//		//		if (rigidBody->id_ == Common::Limits<Common::Index>::Max()) {
	//		//			PAL::Shape::CreateInfoBox shapeCreateInfo{
	//		//				.material_ = PAL::Shape::Material{
	//		//					.staticFriction_ = rigidBody->material_.staticFriction_,
	//		//					.dynamicFriction_ = rigidBody->material_.dynamicFriction_,
	//		//					.restitution_ = rigidBody->material_.restitution_ },
	//		//				.halfExtentX_ = rigidBody->halfExtentX_,
	//		//				.halfExtentY_ = rigidBody->halfExtentY_,
	//		//				.halfExtentZ_ = rigidBody->halfExtentZ_,
	//		//			};
	//		//			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
	//		//			PAL::RigidBody::CreateInfo createInfo{
	//		//				.transform_ = rigidBody->transform_,
	//		//				.mass_ = rigidBody->mass_,
	//		//				.linearDamping_ = rigidBody->linearDamping_,
	//		//				.angularDamping_ = rigidBody->angularDamping_,
	//		//				.shape_ = shape
	//		//			};
	//		//			rigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
	//		//			physicsSubsystem->AddRigidBodyToWorld(rigidBody->id_);
	//		//		}
	//		//		auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBody->id_);
	//		//		rigidBody->SetTransform(rbTransform);
	//		//	}
	//		//}
	//		//return;
	//		//{
	//		//	DynamicRigidBodyCapsule* rigidBodyCapsule = world->GetComponent<DynamicRigidBodyCapsule>(entityId);

	//		//	if (rigidBodyCapsule != nullptr) {
	//		//		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
	//		//		if (rigidBodyCapsule->id_ == Common::Limits<Common::Index>::Max()) {
	//		//			PAL::Shape::CreateInfoCapsule shapeCreateInfo{
	//		//				.material_ = PAL::Shape::Material{
	//		//					.staticFriction_ = rigidBodyCapsule->material_.staticFriction_,
	//		//					.dynamicFriction_ = rigidBodyCapsule->material_.dynamicFriction_,
	//		//					.restitution_ = rigidBodyCapsule->material_.restitution_
	//		//				},
	//		//				.radius_ = rigidBodyCapsule->radius_,
	//		//				.height_ = rigidBodyCapsule->height_
	//		//			};

	//		//			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
	//		//			PAL::RigidBody::CreateInfo createInfo{
	//		//				.transform_ = rigidBodyCapsule->transform_,
	//		//				.mass_ = rigidBodyCapsule->mass_,
	//		//				.linearDamping_ = rigidBodyCapsule->linearDamping_,
	//		//				.angularDamping_ = rigidBodyCapsule->angularDamping_,
	//		//				.shape_ = shape
	//		//			};
	//		//			rigidBodyCapsule->id_ = physicsSubsystem->CreateRigidBody(createInfo);
	//		//			physicsSubsystem->AddRigidBodyToWorld(rigidBodyCapsule->id_);
	//		//		}
	//		//		auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
	//		//		//Delete rotate component
	//		//		rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
	//		//		rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
	//		//		rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
	//		//		rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	//		//		physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
	//		//		rigidBodyCapsule->SetTransform(rbTransform);
	//		//	}
	//		//}

	//		//{
	//		//	StaticRigidBodyCustomMeshShape* staticRigidBody = world->GetComponent<StaticRigidBodyCustomMeshShape>(entityId);

	//		//	if (staticRigidBody != nullptr) {
	//		//		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
	//		//		if (staticRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
	//		//			auto modelGeom = physicsSubsystem->GetGeom(staticRigidBody->geomName_);
	//		//			PAL::Shape::CreateInfoMesh shapeCreateInfo{
	//		//				.material_ = PAL::Shape::Material{
	//		//					.staticFriction_ = staticRigidBody->material_.staticFriction_,
	//		//					.dynamicFriction_ = staticRigidBody->material_.dynamicFriction_,
	//		//					.restitution_ = staticRigidBody->material_.restitution_
	//		//				},
	//		//				.shape_ = *(modelGeom->begin())
	//		//			};

	//		//			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
	//		//			PAL::StaticRigidBody::CreateInfo createInfo{
	//		//				.transform_ = staticRigidBody->transform_,
	//		//				.shape_ = shape
	//		//			};
	//		//			staticRigidBody->id_ = physicsSubsystem->CreateStaticRigidBody(createInfo);
	//		//			physicsSubsystem->AddStaticRigidBodyToWorld(staticRigidBody->id_);
	//		//		}

	//		//		//}
	//		//		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
	//		//		////Delete rotate component
	//		//		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
	//		//		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
	//		//		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
	//		//		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	//		//		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
	//		//		//rigidBodyCapsule->SetTransform(rbTransform);
	//		//	}
	//		//}

	//		//{
	//		//	DynamicRigidBodyCustomMeshShape* dynamicRigidBody = world->GetComponent<DynamicRigidBodyCustomMeshShape>(entityId);

	//		//	if (dynamicRigidBody != nullptr) {
	//		//		auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
	//		//		if (dynamicRigidBody->id_ == Common::Limits<Common::Index>::Max()) {
	//		//			auto modelGeom = physicsSubsystem->GetGeom(dynamicRigidBody->meshName_);
	//		//			PAL::Shape::CreateInfoMesh shapeCreateInfo{
	//		//				.material_ = PAL::Shape::Material{
	//		//					.staticFriction_ = dynamicRigidBody->material_.staticFriction_,
	//		//					.dynamicFriction_ = dynamicRigidBody->material_.dynamicFriction_,
	//		//					.restitution_ = dynamicRigidBody->material_.restitution_
	//		//				},
	//		//				.shape_ = *(modelGeom->begin())
	//		//			};

	//		//			auto shape = physicsSubsystem->CreateShape(shapeCreateInfo);
	//		//			PAL::RigidBody::CreateInfo createInfo{
	//		//				.transform_ = dynamicRigidBody->transform_,
	//		//				.shape_ = shape
	//		//			};
	//		//			dynamicRigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
	//		//			physicsSubsystem->AddRigidBodyToWorld(dynamicRigidBody->id_);
	//		//		}

	//		//		//}
	//		//		//auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
	//		//		////Delete rotate component
	//		//		//rbTransform[0][0] = 1.0f; rbTransform[0][1] = 0.0f; rbTransform[0][2] = 0.0f;
	//		//		//rbTransform[1][0] = 0.0f; rbTransform[1][1] = 1.f; rbTransform[1][2] = 0.0f;
	//		//		//rbTransform[2][0] = 0.0f; rbTransform[2][1] = 0.0f; rbTransform[2][2] = 1.0f;
	//		//		//rbTransform = glm::rotate(rbTransform, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	//		//		//physicsSubsystem->SetRigidBodyTransform(rigidBodyCapsule->id_, rbTransform);
	//		//		//rigidBodyCapsule->SetTransform(rbTransform);
	//		//	}
	//		//}

	//	}


	//	virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override
	//	{
	//		return { ECS::Entity::Filter{}, ECS::Entity::Filter{} };
	//	}

	//	virtual Common::TypeId GetTypeId() const noexcept override {
	//		return Common::TypeInfo<PhysicsSystem>().GetId();
	//	}

	//private:
	//};


	////class CreateRigidBodyCustomMeshShape



}