
#include <OksEngine.ECS.System.hpp>
#include <OksEngine.RigidBody.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>

namespace OksEngine {

	class PhysicsSystem : public ECSSystem {
	public:

		PhysicsSystem(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			RigidBody* rigidBody = world->GetComponent<RigidBody>(entityId);
			if (rigidBody == nullptr) return;
			auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
			if (rigidBody->id_ == Common::Limits<Common::Index>::Max()) {
				PAL::RigidBody::CreateInfo createInfo{
					.transform_ = Math::Matrix4x4f::GetTranslate({0.0f, 10.f, 0.f}),
					.staticFriction_ = rigidBody->staticFriction_,
					.dynamicFriction_ = rigidBody->dynamicFriction_,
					.restitution_ = rigidBody->restitution_,
					.halfExtentX_ = rigidBody->halfExtentX_,
					.halfExtentY_ = rigidBody->halfExtentY_,
					.halfExtentZ_ = rigidBody->halfExtentZ_,
				};
				rigidBody->id_ = physicsSubsystem->CreateRigidBody(createInfo);
				physicsSubsystem->AddRigidBodyToWorld(rigidBody->id_);
			}
			auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBody->id_);
			auto origin = rbTransform.GetOrigin();
			rigidBody->SetTransform(rbTransform);

		}
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsSystem>().GetId();
		}

	private:
	};

	class PhysicsGeometryMapper : public ECSSystem {
	public:
		PhysicsGeometryMapper(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			RigidBody* rigidBody = world->GetComponent<RigidBody>(entityId);
			ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (rigidBody == nullptr) return;
			if (renderGeometry == nullptr) return;
			auto transform = rigidBody->GetTransform();
			auto origin = transform.GetOrigin();
			renderGeometry->modelMatrix_ = Math::Matrix4x4f::GetTranslate(origin);

		}
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsSystem>().GetId();
		}

	};

}