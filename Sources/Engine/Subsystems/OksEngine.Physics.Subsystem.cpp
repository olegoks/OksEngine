
#include <OksEngine.Physics.Subsystem.hpp>
#include <PAL.hpp>
#include <OksEngine.Physics.System.hpp>

namespace OksEngine {


	PhysicsSubsystem::PhysicsSubsystem(const CreateInfo& createInfo) : 
		Subsystem{ 
			Subsystem::Type::Physics,
			createInfo.context_ }, 
			physicsEngine_{ nullptr } { 

		auto& context = GetContext();
		auto ecsWorld = context.GetECSWorld();
		ecsWorld->RegisterSystem<PhysicsSystem>(context);
		ecsWorld->RegisterSystem<PhysicsGeometryMapper>(context);

		physicsEngine_ = std::make_shared<PE::PhysicsEngine>();
		PAL::World::CreateInfo worldCreateInfo{
			.gravity_ = { 0.0, -9.81, 0.0 }
		};
		physicsEngine_->CreateWorld(worldCreateInfo);
	}

	Common::Index PhysicsSubsystem::CreateRigidBody(const PAL::RigidBody::CreateInfo& createInfo) {
		auto rigidBody = physicsEngine_->CreateRigidBody(createInfo);
		rigidBodies_.push_back(rigidBody);
		return rigidBodies_.size() - 1;
	}

	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfo1& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}
	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfo2& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}

	void PhysicsSubsystem::AddRigidBodyToWorld(Common::Index rbIndex) {
		physicsEngine_->GetWorld()->AddRigidBody(rigidBodies_[rbIndex]);
	}

	void PhysicsSubsystem::Update() noexcept {
		physicsEngine_->Simulate(1 / 60.f);
	}


}