
#include <OksEngine.Physics.Subsystem.hpp>
#include <PAL.hpp>
namespace OksEngine {


	PhysicsSubsystem::PhysicsSubsystem(const CreateInfo& createInfo) : 
		Subsystem{ 
			Subsystem::Type::Physics,
			createInfo.context_ }, 
			physicsEngine_{ nullptr } { 
	
		physicsEngine_ = std::make_shared<PE::PhysicsEngine>();
		
	}

	void PhysicsSubsystem::Update() noexcept {

	}


}