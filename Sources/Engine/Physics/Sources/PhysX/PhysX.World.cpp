
#include <PhysX.World.hpp>
#include <PhysX.RigidBody.hpp>
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
		physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
		if (pvdClient) {
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		physx::PxMaterial* material = createInfo.physics_->createMaterial(0.5f, 0.5f, 0.6f);
		//physx::PxInputStream 
		//createInfo.physics_->createTriangleMesh()
		//physx::PxTriangleMeshGeometry
		physx::PxRigidStatic* groundPlane = PxCreatePlane(*createInfo.physics_, physx::PxPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }), *material);
		scene->addActor(*groundPlane);

	}

	void World::Simulate(float ms) {
		OS::AssertMessage(!Math::IsEqual(ms, 0.f), "Invalid simulation time");
		scene_->simulate(ms);
		scene_->fetchResults(true);
	}

	void World::AddRigidBody(std::shared_ptr<PAL::RigidBody> rigidBody) {
		OS::Assert(scene_ != nullptr);
		auto physxRB = std::static_pointer_cast<PhysX::RigidBody>(rigidBody);
		scene_->addActor(*physxRB->GetBody());
		/*PxTransform t{PxVec3(0)};
		PxShape* shape = physics_->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *material);
		for (PxU32 i = 0; i < size; i++) {
			for (PxU32 j = 0; j < size - i; j++) {
				PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
				PxRigidDynamic* body = physics_->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
				scene->addActor(*body);
			}
		}*/
	}
}