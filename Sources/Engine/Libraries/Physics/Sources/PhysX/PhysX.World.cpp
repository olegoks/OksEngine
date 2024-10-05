
#include <PhysX.World.hpp>
#include <PhysX.RigidBody.hpp>
#include <OS.Assert.hpp>
#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
namespace PhysX {

	World::World(const CreateInfo& pxCreateInfo) : PAL::World{ pxCreateInfo.palWorldCreateInfo_ }, physics_{ pxCreateInfo.physics_ }{

		OS::AssertMessage(pxCreateInfo.physics_ != nullptr, "PxPhysics is not initialized.");

		physx::PxSceneDesc sceneDescription(pxCreateInfo.physics_->getTolerancesScale());
		const Math::Vector3f gravity = pxCreateInfo.palWorldCreateInfo_.gravity_;
		sceneDescription.gravity = physx::PxVec3(gravity.GetX(), gravity.GetY(), gravity.GetZ());
		physx::PxDefaultCpuDispatcher* dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDescription.cpuDispatcher = dispatcher;
		sceneDescription.filterShader = physx::PxDefaultSimulationFilterShader;
		physx::PxScene* scene = pxCreateInfo.physics_->createScene(sceneDescription);
		OS::Assert(scene != nullptr);
		scene_ = scene;
		physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
		if (pvdClient) {
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		//physx::PxMaterial* material = pxCreateInfo.physics_->createMaterial(0.5f, 0.5f, 0.6f);
		//physx::PxRigidStatic* groundPlane = PxCreatePlane(*pxCreateInfo.physics_, physx::PxPlane({ 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }), *material);
		//scene->addActor(*groundPlane);

	}

	PAL::DynamicRigidBody::Id World::CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo)
	{
		PhysX::DynamicRigidBody::CreateInfo physxCreateInfo{
				.palCreateInfo_ = drbCreateInfo,
				.physics_ = physics_
		};
		auto drbPtr = std::make_shared<PhysX::DynamicRigidBody>(physxCreateInfo);
		return GenerateId(drbPtr);
	}

	PAL::StaticRigidBody::Id World::CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo)
	{
		PhysX::StaticRigidBody::CreateInfo physxCreateInfo{
			.palCreateInfo_ = srbCreateInfo,
			.physics_ = physics_
		};
		auto srbPtr = std::make_shared<PhysX::StaticRigidBody>(physxCreateInfo);
		return GenerateId(srbPtr);
	}

	void World::Simulate(float ms) {
		OS::AssertMessage(!Math::IsEqual(ms, 0.f), "Invalid simulation time");
		scene_->simulate(ms);
		scene_->fetchResults(true);
	}


	void World::AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) {
		OS::Assert(scene_ != nullptr);
		auto drbPtr = GetRigidBodyById(drbId);
		auto physxRB = std::static_pointer_cast<PhysX::DynamicRigidBody>(drbPtr);
		scene_->addActor(*physxRB->GetBody());
		/*using namespace physx;
		PxTransform t{PxVec3(0)};
		physx::PxMaterial* material = physics_->createMaterial(
			0.5,
			0.5,
			0.5);
		OS::AssertMessage(material != nullptr, "Error while creating physx material.");
		PxShape* shape = physics_->createShape(PxBoxGeometry(1, 1, 1), *material);
		for (PxU32 i = 0; i < 5; i++) {
			for (PxU32 j = 0; j < 5 - i; j++) {
				PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(5 - i), PxReal(i * 2 + 1), 0) * 0.5);
				PxRigidDynamic* body = physics_->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
				scene_->addActor(*body);
			}
		}*/
	}

	void World::AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) {
		OS::Assert(scene_ != nullptr);
		auto srbPtr = GetRigidBodyById(srbId);
		auto physxRB = std::static_pointer_cast<PhysX::StaticRigidBody>(srbPtr);
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