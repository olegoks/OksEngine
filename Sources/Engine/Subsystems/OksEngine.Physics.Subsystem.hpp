
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
		
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoBox& createInfo);
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo);

		void AddRigidBodyToWorld(Common::Index rbIndex);
		[[nodiscard]]
		glm::mat4 GetRigidBodyTransform(Common::Index rbIndex) {
			return rigidBodies_[rbIndex]->GetTransform();
		}

		[[nodiscard]]
		void  SetRigidBodyTransform(Common::Index rbIndex, const glm::mat4& transform) {
			return rigidBodies_[rbIndex]->SetTransform(transform);
		}

		void ApplyForce(Common::Index rbIndex, const glm::vec3& direction, float force);

		virtual void Update() noexcept override;


	private:
		std::vector<std::shared_ptr<PAL::RigidBody>> rigidBodies_;
		std::shared_ptr<PAL::World> world_ = nullptr;
		std::shared_ptr<PE::PhysicsEngine> physicsEngine_ = nullptr;
	};

}