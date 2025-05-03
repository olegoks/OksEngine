
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <PAL.hpp>
#include <Physics/OksEngine.Physics.Systems.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>

#include "Geometry.Model.hpp"

namespace OksEngine {


	PhysicsSubsystem::PhysicsSubsystem(const CreateInfo& createInfo) :
		Subsystem{
			Subsystem::Type::Physics,
			createInfo.context_ },
			physicsEngine_{ nullptr } {

		auto& context = GetContext();
		//auto ecsWorld = context.GetECSWorld();

		//ecsWorld->RegisterSystem<CreateStaticRigidBody>(context);
		//ecsWorld->RegisterSystem<CreatePhysicsShapeForStaticRigidBody>(context);
		//ecsWorld->RegisterSystem<CreatePhysicsShapeForDynamicRigidBody>(context);
		//ecsWorld->RegisterSystem<CreateDynamicRigidBody>(context);
		// 

		physicsEngine_ = std::make_shared<PE::PhysicsEngine>();
		PAL::World::CreateInfo worldCreateInfo{
			.gravity_ = { 0.0, -9.81, 0.0 }
		};
		world_ = physicsEngine_->GetAPI()->CreateWorld(worldCreateInfo);
	}



	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoBox& createInfo) {
		auto shape = physicsEngine_->GetAPI()->CreateShape(createInfo);
		return shape;
	}
	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) {
		auto shape = physicsEngine_->GetAPI()->CreateShape(createInfo);
		return shape;
	}

	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoMesh& createInfo) {
		auto shape = physicsEngine_->GetAPI()->CreateShape(createInfo);
		return shape;
	}
	//void PhysicsSubsystem::AddRigidBodyToWorld(PAL::DynamicRigidBody::Id rbIndex) {
	//	physicsEngine_->GetWorld()->AddRigidBody(rbIndex);
	//}

	//void PhysicsSubsystem::AddStaticRigidBodyToWorld(PAL::StaticRigidBody::Id rbIndex) {
	//	physicsEngine_->GetWorld()->AddStaticRigidBody(rbIndex);
	//}

	//void PhysicsSubsystem::ApplyForce(Common::Index rbIndex, const glm::vec3& direction, float force) {
	//	physicsEngine_->GetWorld()->ApplyForce(rbIndex, direction, force);
	//}

	//void PhysicsSubsystem::SetVelocity(Common::Index rbIndex, const glm::vec3& direction, float velocity) {
	//	physicsEngine_->GetWorld()->SetVelocity(rbIndex, direction, velocity);
	//}



	void PhysicsSubsystem::Update() noexcept {
		


	}


}
