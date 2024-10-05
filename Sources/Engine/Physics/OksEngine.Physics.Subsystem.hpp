
#pragma once 

#include <Common/OksEngine.Subsystem.hpp>
#include <PAL.API.hpp>
#include <PE.PhysicsEngine.hpp>
#include <Geometry.Model.hpp>
#include <PAL.RigidBody.hpp>

namespace OksEngine {

	class PhysicsSubsystem : public Subsystem {
	public:


		struct CreateInfo {
			Context& context_;
		};

		PhysicsSubsystem(const CreateInfo& createInfo);

		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoBox& createInfo);
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo);
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoMesh& createInfo);
		//void AddRigidBodyToWorld(PAL::DynamicRigidBody::Id rbIndex);
		//void AddStaticRigidBodyToWorld(PAL::StaticRigidBody::Id rbIndex);

		std::shared_ptr<PAL::World> GetWorld() noexcept {
			OS::Assert(world_ != nullptr);
			return world_;
		}

		virtual void Update() noexcept override;


	private:
		std::shared_ptr<PAL::World> world_ = nullptr;
		std::shared_ptr<PE::PhysicsEngine> physicsEngine_ = nullptr;
	};

}