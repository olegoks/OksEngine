
#include <PhysX.World.hpp>
#include <PhysX.RigidBody.hpp>
#include <PhysX.Constraint.hpp>
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
		scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0);
		OS::Assert(scene != nullptr);
		scene_ = scene;
		physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
		if (pvdClient) {
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		

		controllerManager_ = PxCreateControllerManager(*scene);
		ASSERT(controllerManager_ != nullptr);
	}

	void World::SetDebugRenderParameters(DebugRenderParameters parameter, bool value) noexcept  {
		if (parameter == DebugRenderParameters::eCOLLISION_FNORMALS) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_FNORMALS, (value)?(1.0):(0.0));
		}
		else if (parameter == DebugRenderParameters::eENABLE) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCOLLISION_AABBS) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCOLLISION_EDGES) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_EDGES, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eBODY_AXES) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eBODY_AXES, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eACTOR_AXES) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eWORLD_AXES) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eWORLD_AXES, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCOLLISION_STATIC) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_STATIC, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCOLLISION_DYNAMIC) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_DYNAMIC, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCONTACT_POINT) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eCONTACT_NORMAL) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_NORMAL, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eJOINT_LOCAL_FRAMES) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, (value) ? (1.0) : (0.0));
		}
		else if (parameter == DebugRenderParameters::eJOINT_LIMITS) {
			scene_->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, (value) ? (1.0) : (0.0));
		}
		else {
			NOT_IMPLEMENTED();
		}
	}

	PAL::DynamicRigidBody::Id World::CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo)
	{
		PhysX::DynamicRigidBody::CreateInfo physxCreateInfo{
				.palCreateInfo_ = drbCreateInfo,
				.physics_ = physics_
		};
		auto drbPtr = std::make_shared<PhysX::DynamicRigidBody>(physxCreateInfo);
		return GenerateRbId(drbPtr);
	}

	PAL::StaticRigidBody::Id World::CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo)
	{
		PhysX::StaticRigidBody::CreateInfo physxCreateInfo{
			.palCreateInfo_ = srbCreateInfo,
			.physics_ = physics_
		};
		auto srbPtr = std::make_shared<PhysX::StaticRigidBody>(physxCreateInfo);
		return GenerateRbId(srbPtr);
	}

	[[nodiscard]]
	physx::PxRigidActor* World::GetRigidBodyActor(PAL::RigidBody::Id rbId) {

		auto firstRb = Common::pointer_cast<PAL::RigidBody>(GetRigidBodyById(rbId));

		if (firstRb->GetType() == PAL::RigidBody::Type::Dynamic) {
			return Common::pointer_cast<PhysX::DynamicRigidBody>(firstRb)->GetBody();
		}
		else {
			return Common::pointer_cast<PhysX::StaticRigidBody>(firstRb)->GetBody();
		}

	}

	PAL::Constraint::Id World::CreateFixedConstraint(const PAL::FixedConstraint::CI& ci) {

		physx::PxRigidActor* physxFirst = GetRigidBodyActor(ci.first_);
		physx::PxRigidActor* physxSecond = GetRigidBodyActor(ci.second_);

		physx::PxTransform firstTR{ convertToPxMat44(ci.firstRelativeTr_)};
		physx::PxTransform secondTR{ convertToPxMat44(ci.secondRelativeTr_) };

		PhysX::FixedConstraint::CreateInfo physxCI{
			.physics_ = physics_,
			.first_ = physxFirst,
			.firstTr_ = firstTR,
			.second_ = physxSecond,
			.secondTr_ = secondTR
		};

		auto constraintPtr = std::make_shared<PhysX::FixedConstraint>(physxCI);

		if (ci.isBreakable_) {
			constraintPtr->SetBreakForce(ci.breakForce_);
		}


		return GenerateConstraintId(constraintPtr);

	}

	PAL::Constraint::Id World::CreateRevoluteConstraint(const PAL::RevoluteConstraint::CI& ci) {

		physx::PxRigidActor* physxFirst = GetRigidBodyActor(ci.first_);
		physx::PxRigidActor* physxSecond = GetRigidBodyActor(ci.second_);

		physx::PxTransform firstTR{ convertToPxMat44(ci.firstRelativeTr_) };
		physx::PxTransform secondTR{ convertToPxMat44(ci.secondRelativeTr_) };

		PhysX::RevoluteConstraint::CreateInfo physxCI{
			.physics_ = physics_,
			.first_ = physxFirst,
			.firstTr_ = firstTR,
			.second_ = physxSecond,
			.secondTr_ = secondTR
		};

		auto constraintPtr = std::make_shared<PhysX::RevoluteConstraint>(physxCI);

		if (ci.isBreakable_) {
			constraintPtr->SetBreakForce(ci.breakForce_);
		}

		if (ci.invMassScaleFirst_ != Common::Limits<float>::Max()) {
			constraintPtr->SetInvMassScale0(ci.invMassScaleFirst_);
		}
		if (ci.invMassScaleSecond_ != Common::Limits<float>::Max()) {
			constraintPtr->SetInvMassScale1(ci.invMassScaleSecond_);
		}

		return GenerateConstraintId(constraintPtr);

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

		physx::PxSimulationStatistics statistics;
		scene_->getSimulationStatistics(statistics);
		//statistics.
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

	glm::quat World::GetRigidBodyRotation(PAL::DynamicRigidBody::Id drbId) {
		auto rb = GetRigidBodyById(drbId);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(rb->GetTransform(), scale, rotation, translation, skew, perspective);

		return rotation;
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