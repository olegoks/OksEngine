
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
					.transform_ = rigidBody->transform_,
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
			if (rigidBody == nullptr) return;
			ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (renderGeometry == nullptr) return;
			const auto& rigidBodyTransform = rigidBody->GetTransform();
			renderGeometry->SetTransform(rigidBodyTransform);

		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsSystem>().GetId();
		}

	};

}