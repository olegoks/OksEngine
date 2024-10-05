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

		virtual PAL::DynamicRigidBody::Id CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo) override;
		virtual PAL::StaticRigidBody::Id CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo) override;

		virtual void AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) override;

		virtual void AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) override;

		virtual void Simulate(float ms) override;

	private:
		physx::PxScene* scene_ = nullptr;
		physx::PxPhysics* physics_ = nullptr;
	};

}