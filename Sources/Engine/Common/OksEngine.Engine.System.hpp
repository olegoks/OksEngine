#pragma once 


#include <OksEngine.ECS.System.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>
#include <Render/OksEngine.Camera.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/OksEngine.AttachedCamera.hpp>

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
			auto* rb = world->GetComponent<DynamicRigidBodyCapsule>(entityId);
			if (rb == nullptr) return;
			const glm::mat4 cameraTransform = rb->GetTransform()/* * attachCamera->offset_*/;
			camera->Transform(cameraTransform);
		}

		virtual ECS::Entity::Filter GetFilter() const noexcept override
		{
			return ECS::Entity::Filter{}.Include<AttachedCamera>();
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
			DynamicRigidBodyBox* rigidBody = world->GetComponent<DynamicRigidBodyBox>(entityId);
			if (rigidBody == nullptr) return;
			ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (renderGeometry == nullptr) return;
			const auto& rigidBodyTransform = rigidBody->GetTransform();
			renderGeometry->SetTransform(rigidBodyTransform);

		}

		virtual ECS::Entity::Filter GetFilter() const noexcept override{
			return ECS::Entity::Filter{}.Include<DynamicRigidBodyBox>().Include<ImmutableRenderGeometry>();
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsGeometryMapper>().GetId();
		}

	};

}