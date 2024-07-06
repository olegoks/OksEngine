
#include <OksEngine.ECS.System.hpp>
#include <OksEngine.RigidBody.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>

namespace OksEngine {

	class PhysicsSystem : public ECSSystem {
	public:

		PhysicsSystem(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			{
				RigidBodyBox* rigidBody = world->GetComponent<RigidBodyBox>(entityId);

				if (rigidBody == nullptr) return;
				auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
				if (rigidBody->id_ == Common::Limits<Common::Index>::Max()) {
					PAL::RigidBody::CreateInfo createInfo{
						.transform_ = rigidBody->transform_,
						.mass_ = rigidBody->mass_,
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

			{
				RigidBodyCapsule* rigidBodyCapsule = world->GetComponent<RigidBodyCapsule>(entityId);

				if (rigidBodyCapsule == nullptr) return;
				auto physicsSubsystem = GetContext().GetPhysicsSubsystem();
				if (rigidBodyCapsule->id_ == Common::Limits<Common::Index>::Max()) {
					PAL::Shape::CreateInfo2 createInfo{
						.radius_ = rigidBodyCapsule->radius_,
						.height_ = rigidBodyCapsule->height_
					};

					auto shape = physicsSubsystem->CreateShape(createInfo);
					
				}
				auto rbTransform = physicsSubsystem->GetRigidBodyTransform(rigidBodyCapsule->id_);
				rigidBodyCapsule->SetTransform(rbTransform);
			}
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
			RigidBodyBox* rigidBody = world->GetComponent<RigidBodyBox>(entityId);
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