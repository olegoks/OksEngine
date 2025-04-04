#pragma once 
#include <PAL.API.hpp>
#include <OS.Assert.hpp>
#include <PhysX.World.hpp>
#include <PhysX.RigidBody.hpp>
#include <PxPhysicsAPI.h>
#include <physx\cooking\Pxc.h>
#include <PxPhysics.h>
#include <PhysX.World.hpp>
#include <PhysX.Shape.hpp>

namespace PhysX {

	class API : public PAL::API {
	public:

		// Custom error callback class
		class CustomErrorCallback : public physx::PxErrorCallback {
		public:
			void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override {
				OS::LogError("physx", {"{}", message });
			}
		};

		API() {
			using namespace physx;
			static physx::PxDefaultErrorCallback gDefaultErrorCallback;
			static physx::PxDefaultAllocator gDefaultAllocatorCallback;

			foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
				errorCallback_);
			OS::Assert(foundation_ != nullptr);

			bool recordMemoryAllocations = true;

			pvd_ = physx::PxCreatePvd(*foundation_); // Visual debugger 
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd_->connect(*transport, PxPvdInstrumentationFlag::eALL);


			physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation_,
				PxTolerancesScale(), recordMemoryAllocations, pvd_);
			OS::Assert(physics_ != nullptr);
			//physx::PxCreateCooking(PX_PHYSICS_VERSION, *foundation_, PxCookingParams(physx::PxTolerancesScale()));
			/*OS::Assert(mPhysics != nullptr);
			if (!mCooking)
				fatalError("PxCreateCooking failed!");*/
			if (!PxInitExtensions(*physics_, pvd_))
				OS::AssertFail();

			//PxSceneDesc sceneDescription(physics_->getTolerancesScale());
			//sceneDescription.gravity = PxVec3(0.0f, -9.81f, 0.0f);
			//PxDefaultCpuDispatcher* dispatcher = PxDefaultCpuDispatcherCreate(2);
			//sceneDescription.cpuDispatcher = dispatcher;
			//sceneDescription.filterShader = PxDefaultSimulationFilterShader;
			//PxScene* scene = physics_->createScene(sceneDescription);

			//PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
			//if (pvdClient) {
			//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			//}

			//PxMaterial* material = physics_->createMaterial(0.5f, 0.5f, 0.6f);
			//PxRigidStatic* groundPlane = PxCreatePlane(*physics_, PxPlane(0, 1, 0, 10), *material);
			//scene->addActor(*groundPlane);

			//PxU32 size = 50;
			//float halfExtent = .5f;
			//PxTransform t{PxVec3(0)};
			//PxShape* shape = physics_->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *material);
			//for (PxU32 i = 0; i < size; i++) {
			//	for (PxU32 j = 0; j < size - i; j++) {
			//		PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			//		PxRigidDynamic* body = physics_->createRigidDynamic(t.transform(localTm));
			//		body->attachShape(*shape);
			//		PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
			//		scene->addActor(*body);
			//	}
			//}

			//while (1) {
			//	scene->simulate(1.0 / 60.f);
			//	scene->fetchResults(true);
			//}
		}


		virtual std::shared_ptr<PAL::World> 
		CreateWorld(const PAL::World::CreateInfo& createInfo) override {
			PhysX::World::CreateInfo physxCreateInfo{
				.palWorldCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::World>(physxCreateInfo);
		}

		virtual std::shared_ptr<PAL::DynamicRigidBody> 
		CreateRigidBody(const PAL::DynamicRigidBody::CreateInfo& createInfo) override {
			PhysX::DynamicRigidBody::CreateInfo physxCreateInfo{
				.palCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::DynamicRigidBody>(physxCreateInfo);
		}

		virtual std::shared_ptr<PAL::StaticRigidBody>
			CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& createInfo) override {
			PhysX::StaticRigidBody::CreateInfo physxCreateInfo{
				.palCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::StaticRigidBody>(physxCreateInfo);
		}

		virtual std::shared_ptr<PAL::Shape>
		CreateShape(const PAL::Shape::CreateInfoBox& createInfo)override {
			PhysX::Shape::CreateInfoBox physxCreateInfo{
				.palCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::Shape>(physxCreateInfo);
		}

		virtual std::shared_ptr<PAL::Shape>
		CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) override {
			PhysX::Shape::CreateInfoCapsule physxCreateInfo{
				.palCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::Shape>(physxCreateInfo);
		}

		virtual std::shared_ptr<PAL::Shape>
			CreateShape(const PAL::Shape::CreateInfoMesh& createInfo) override {
			PhysX::Shape::CreateInfoMesh physxCreateInfo{
				.palCreateInfo_ = createInfo,
				.physics_ = physics_
			};
			return std::make_shared<PhysX::Shape>(physxCreateInfo);
		}

		~API() {
			physics_->release();
			foundation_->release();
		}

	private:
		CustomErrorCallback errorCallback_;
		physx::PxFoundation* foundation_ = nullptr;
		physx::PxPhysics* physics_ = nullptr;
		//physx::PxCooking* cooking_ = nullptr;
		physx::PxPvd* pvd_ = nullptr;
	};
}