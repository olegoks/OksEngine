
#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <PAL.API.hpp>
#include <PE.PhysicsEngine.hpp>

namespace OksEngine {

	class PhysicsSubsystem : public Subsystem {
	public:


		struct CreateInfo {
			Context& context_;
		};

		PhysicsSubsystem(const CreateInfo& createInfo);

		Common::Index CreateRigidBody(const PAL::RigidBody::CreateInfo& createInfo);
		void AddRigidBodyToWorld(Common::Index rbIndex);
		[[nodiscard]]
		glm::mat4 GetRigidBodyTransform(Common::Index rbIndex) {
			return rigidBodies_[rbIndex]->GetTransform();
		}
		virtual void Update() noexcept override;


	private:
		std::vector<std::shared_ptr<PAL::RigidBody>> rigidBodies_;
		std::shared_ptr<PAL::World> world_ = nullptr;
		std::shared_ptr<PE::PhysicsEngine> physicsEngine_ = nullptr;
	};

}