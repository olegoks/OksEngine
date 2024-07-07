#pragma once 


#include <OksEngine.ECS.System.hpp>
#include <OksEngine.RigidBody.hpp>
#include <OksEngine.Camera.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>
#include <OksEngine.AttachedCamera.hpp>

namespace OksEngine {

	class AttachCameraSystem : public ECSSystem {
	public:
		AttachCameraSystem(Context& context) : ECSSystem{ context } {

		}

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			auto* attachCamera = world->GetComponent<AttachedCamera>(entityId);
			if (attachCamera == nullptr) return;
			auto* camera = world->GetComponent<Camera>(entityId);
			if (camera == nullptr) return;
			auto* rb = world->GetComponent<RigidBodyCapsule>(entityId);
			if (rb == nullptr) return;
			const glm::mat4 cameraTransform = rb->GetTransform()/* * attachCamera->offset_*/;
			camera->Transform(cameraTransform);
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<AttachCameraSystem>().GetId();
		}


	};


	class PhysicsGeometryMapper : public ECSSystem {
	public:
		PhysicsGeometryMapper(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			RigidBodyBox* rigidBody = world->GetComponent<RigidBodyBox>(entityId);
			if (rigidBody == nullptr) return;
			ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (renderGeometry == nullptr) return;
			const auto& rigidBodyTransform = rigidBody->GetTransform();
			renderGeometry->SetTransform(rigidBodyTransform);

		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsGeometryMapper>().GetId();
		}

	};

}