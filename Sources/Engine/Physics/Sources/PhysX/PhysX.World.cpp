
#include <PhysX.World.hpp>
#include <OS.Assert.hpp>
#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
namespace PhysX {
	
	World::World(const CreateInfo& createInfo) : PAL::World{ createInfo.palWorldCreateInfo_ }{

		OS::AssertMessage(createInfo.physics_ != nullptr, "PxPhysics is not initialized.");

		physx::PxSceneDesc sceneDescription(createInfo.physics_->getTolerancesScale());
		const Math::Vector3f gravity = createInfo.palWorldCreateInfo_.gravity_;
		sceneDescription.gravity = physx::PxVec3(gravity.GetX(), gravity.GetY(), gravity.GetZ());
		physx::PxDefaultCpuDispatcher* dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDescription.cpuDispatcher = dispatcher;
		sceneDescription.filterShader = physx::PxDefaultSimulationFilterShader;
		physx::PxScene* scene = createInfo.physics_->createScene(sceneDescription);
		OS::Assert(scene != nullptr);
		scene_ = scene;

	}
}