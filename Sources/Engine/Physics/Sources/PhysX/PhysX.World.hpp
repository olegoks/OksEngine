#pragma once 
#include <PAL.World.hpp>
#include <PxScene.h>
namespace PhysX {

	class World : public PAL::World {
	public:

		struct CreateInfo {
			PAL::World::CreateInfo palWorldCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		World(const CreateInfo& createInfo);

		virtual void AddRigidBody(std::shared_ptr<PAL::RigidBody> rigidBody) override;
		virtual void Simulate(float ms) override;

	private:
		physx::PxScene* scene_ = nullptr;
	};

}