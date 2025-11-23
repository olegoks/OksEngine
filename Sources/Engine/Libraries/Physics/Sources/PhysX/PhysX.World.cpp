
#include <PhysX.World.hpp>
#include <PhysX.RigidBody.hpp>
#include <OS.Assert.hpp>
#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
namespace PhysX {

	World::World(const CreateInfo& pxCreateInfo) 
		: 
		PAL::World{ pxCreateInfo.palWorldCreateInfo_ },
		physics_{ pxCreateInfo.physics_ }{

		ASSERT_FMSG(pxCreateInfo.physics_ != nullptr, "PxPhysics is not initialized.");

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
		controllerManager_ = PxCreateControllerManager(*scene);

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

	PAL::CapsuleController::Id World::CreateCapsuleController(const PAL::CapsuleController::CreateInfo& srbCreateInfo)
	{
		physx::PxMaterial* material = physics_->createMaterial(0.5f, 0.5f, 0.6f);

		physx::PxCapsuleControllerDesc desc;
		desc.height = 2.0f;        
		desc.radius = 0.5f;        
		desc.position = physx::PxExtendedVec3(srbCreateInfo.position_.x, srbCreateInfo.position_.y, srbCreateInfo.position_.z); 
		desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
		desc.material = material;
		physx::PxCapsuleController* controller = static_cast<physx::PxCapsuleController*>(controllerManager_->createController(desc));
		
		//controller->move()
		std::shared_ptr<physx::PxCapsuleController> ptr{ controller, [](physx::PxCapsuleController* ctrl) {
			if (ctrl) {
				ctrl->release(); // Используем метод release() вместо delete
			}
			} };
		return GenerateCapsuleControllerId(ptr);
	}



	void World::Simulate(float ms) {
		ASSERT_FMSG(!Math::IsEqual(ms, 0.f), "Invalid simulation time");
		scene_->simulate(ms);
		scene_->fetchResults(true);
	}

	void World::ApplyForce(PAL::DynamicRigidBody::Id drbId) {

		auto drbPtr = GetRigidBodyById(drbId);
		auto physxRB = Common::pointer_cast<PhysX::DynamicRigidBody>(drbPtr);
		physxRB->ApplyForce({ 0, 1, 0 }, 10);

	}

	void World::AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) {
		OS::Assert(scene_ != nullptr);
		auto drbPtr = GetRigidBodyById(drbId);
		auto physxRB = Common::pointer_cast<PhysX::DynamicRigidBody>(drbPtr);
		scene_->addActor(*physxRB->GetBody());
		/*using namespace physx;
		PxTransform t{PxVec3(0)};
		physx::PxMaterial* material = physics_->createMaterial(
			0.5,
			0.5,
			0.5);
		ASSERT_FMSG(material != nullptr, "Error while creating physx material.");
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

	Math::Vector3f World::GetRigidBodyPosition(PAL::DynamicRigidBody::Id drbId) {

		auto rb = GetRigidBodyById(drbId);
	
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(rb->GetTransform(), scale, rotation, translation, skew, perspective);

		return { translation.x, translation.y, translation.z };


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