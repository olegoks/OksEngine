
#include <OksEngine.Physics.Subsystem.hpp>
#include <PAL.hpp>
namespace OksEngine {


	PhysicsSubsystem::PhysicsSubsystem(const CreateInfo& createInfo) : 
		Subsystem{ 
			Subsystem::Type::Physics,
			createInfo.context_ } { 
		
		std::shared_ptr<PAL::API> api = PAL::CreateAPI();
		
	}

	void PhysicsSubsystem::Update() noexcept {

	}


}