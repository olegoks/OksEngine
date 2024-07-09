
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

	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoBox& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}
	std::shared_ptr<PAL::Shape> PhysicsSubsystem::CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) {
		auto shape = physicsEngine_->CreateShape(createInfo);
		return shape;
	}

	void PhysicsSubsystem::AddRigidBodyToWorld(Common::Index rbIndex) {
		physicsEngine_->GetWorld()->AddRigidBody(rigidBodies_[rbIndex]);
	}

	void PhysicsSubsystem::ApplyForce(Common::Index rbIndex, const glm::vec3& direction, float force) {
		rigidBodies_[rbIndex]->ApplyForce(direction, force);
	}

	void PhysicsSubsystem::Update() noexcept {
		using namespace std::chrono_literals;
		static std::chrono::high_resolution_clock::time_point previousUpdate = std::chrono::high_resolution_clock::now();
		static std::chrono::high_resolution_clock::duration remainder = 0ms;
		const auto simulationGranularity = 4ms;
		const auto now = std::chrono::high_resolution_clock::now();
		const auto delta = (now - previousUpdate);
		const auto toSimulate = delta + remainder;

		if (toSimulate > simulationGranularity) {
			physicsEngine_->Simulate(4.f / 1000.f);
			remainder = toSimulate - 4ms;
			previousUpdate = now;
		}
		


		
	}


}