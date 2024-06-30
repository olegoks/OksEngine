
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
		
	}

	void PhysicsSubsystem::Update() noexcept {

	}


}