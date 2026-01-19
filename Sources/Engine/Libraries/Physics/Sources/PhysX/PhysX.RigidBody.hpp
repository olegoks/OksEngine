#pragma once 

#include <PAL.RigidBody.hpp>

#include <Math.Scalar.hpp>
#include <PxPhysicsAPI.h>
#include <PxPhysics.h>

#include <Geometry.Shapes.hpp>

#include <PhysX.Common.hpp>
#include <PhysX.Shape.hpp>
#include <physx/PxRigidDynamic.h>
#include <physx/foundation/PxMat44.h>
#include <physx/extensions/PxRigidBodyExt.h>

namespace PhysX {

	class DynamicRigidBody : public PAL::DynamicRigidBody {
	public:
		struct CreateInfo {
			PAL::DynamicRigidBody::CreateInfo palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
			
		};

		DynamicRigidBody(const CreateInfo& createInfo) : PAL::DynamicRigidBody{ createInfo.palCreateInfo_ }{

			physx::PxMat44 pxMatrix = convertToPxMat44(createInfo.palCreateInfo_.rbCreateInfo_.transform_);
			physx::PxTransform t{ pxMatrix };
			auto shape = std::static_pointer_cast<PhysX::Shape>(createInfo.palCreateInfo_.rbCreateInfo_.shape_);
			body_ = createInfo.physics_->createRigidDynamic(t);
			ASSERT_FMSG(body_ != nullptr,
				"Error while creating rigid body.");
			
			body_->attachShape(*shape->GetPxShape());
			body_->setMaxLinearVelocity(100);
			body_->setLinearDamping(createInfo.palCreateInfo_.linearDamping_);
			body_->setAngularDamping(createInfo.palCreateInfo_.angularDamping_);
			body_->setName(createInfo.palCreateInfo_.rbCreateInfo_.name_.c_str());
			body_->setMass(createInfo.palCreateInfo_.mass_);
			

			body_->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
			//body_->setmass

			physx::PxRigidDynamicLockFlags axisLockFlags;
			{

				if (createInfo.palCreateInfo_.lockLinearX_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
				}
				if (createInfo.palCreateInfo_.lockLinearY_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
				}
				if (createInfo.palCreateInfo_.lockLinearZ_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
				}

				if (createInfo.palCreateInfo_.lockAngularX_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
				}
				if (createInfo.palCreateInfo_.lockAngularY_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
				}
				if (createInfo.palCreateInfo_.lockAngularZ_) {
					axisLockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
				}
				body_->setRigidDynamicLockFlags(axisLockFlags);
			}


			physx::PxVec3 massCenter = { 0, 0, 0 };
			bool massAndInertiaUpdated = physx::PxRigidBodyExt::setMassAndUpdateInertia(*body_, createInfo.palCreateInfo_.mass_);
			ASSERT(massAndInertiaUpdated);
		}

		[[nodiscard]]
		physx::PxRigidDynamic* GetBody() {
			OS::Assert(body_ != nullptr);
			return body_;
		}

		[[nodiscard]]
		const physx::PxRigidDynamic* GetBody() const {
			OS::Assert(body_ != nullptr);
			return body_;
		}

		virtual const glm::mat4 GetTransform() override {

			const physx::PxTransform transform = GetBody()->getGlobalPose();
			const physx::PxMat44 pxMatrix(transform);

			glm::mat4 glmMatrix = convertToGlmMat4(pxMatrix);
			return glmMatrix;
		}

		[[nodiscard]]
		virtual void SetTransform(const glm::mat4& transform) override {
			const physx::PxMat44 pxMatrix = convertToPxMat44(transform);
			const physx::PxTransform pxTransform{ pxMatrix };
			GetBody()->setGlobalPose(pxTransform);

		}

		virtual glm::vec3 GetLinearVelocity() const override {

			const physx::PxVec3 linearVelocity = GetBody()->getLinearVelocity();

			return glm::vec3{ linearVelocity.x, linearVelocity.y, linearVelocity.z };

		}

		virtual void SetLinearVelocity(const glm::vec3& direction, float velocity) override {

			glm::vec3 velocityVector = glm::normalize(direction) * velocity;

			GetBody()->setLinearVelocity({ velocityVector.x, velocityVector.y, velocityVector.z });
		}

		virtual void SetAngularVelocity(const glm::vec3& axis, float angleInDegrees) override {

			float angleDegrees = 5.0f;
			float angleRadians = angleDegrees * 3.14 / 180.0f;

			physx::PxVec3 rotation{axis.x, axis.y, axis.z};
			rotation *= angleInDegrees;
			GetBody()->setAngularVelocity(rotation);

		}

		virtual void ApplyForce(const glm::vec3& direction, float force) override {
			const glm::vec3 directionalForce = glm::normalize(direction) * force;
			GetBody()->addForce({ directionalForce.x, directionalForce.y, directionalForce.z });
		}

		//virtual void SetVelocity(const glm::vec3& direction, float velocity) override {
		//	if (Math::IsEqual(velocity, 0.0)) {
		//		GetBody()->setLinearVelocity({ 0.f, 0.f, 0.f });
		//	}
		//	else {
		//		const glm::vec3 directionalForce = glm::normalize(direction) * velocity;
		//		GetBody()->setLinearVelocity({ directionalForce.x, directionalForce.y, directionalForce.z });
		//	}
		//}


	private:
		physx::PxRigidDynamic* body_ = nullptr;

	};



	class StaticRigidBody : public PAL::StaticRigidBody {
	public:
		struct CreateInfo {
			PAL::StaticRigidBody::CreateInfo palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		StaticRigidBody(const CreateInfo& createInfo) : PAL::StaticRigidBody{ createInfo.palCreateInfo_ }{

			physx::PxMat44 pxMatrix = convertToPxMat44(createInfo.palCreateInfo_.rbCreateInfo_.transform_);
			physx::PxTransform t{ pxMatrix };
			auto shape = std::static_pointer_cast<PhysX::Shape>(createInfo.palCreateInfo_.rbCreateInfo_.shape_);
			body_ = createInfo.physics_->createRigidStatic(t);
			ASSERT_FMSG(body_ != nullptr,
				"Error while creating rigid body.");
			body_->attachShape(*shape->GetPxShape());
			body_->setName(createInfo.palCreateInfo_.rbCreateInfo_.name_.c_str());

			body_->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
		}

		[[nodiscard]]
		physx::PxRigidStatic* GetBody() {
			OS::Assert(body_ != nullptr);
			return body_;
		}

		virtual void SetTransform(const glm::mat4& transform) override {

			const physx::PxMat44 pxMatrix = convertToPxMat44(transform);
			const physx::PxTransform pxTransform{ pxMatrix };
			GetBody()->setGlobalPose(pxTransform);
		}

		virtual const glm::mat4 GetTransform() override {

			const physx::PxTransform transform = GetBody()->getGlobalPose();
			const physx::PxMat44 pxMatrix(transform);

			glm::mat4 glmMatrix = convertToGlmMat4(pxMatrix);
			return glmMatrix;
		}


	private:
		physx::PxMaterial* material_ = nullptr;
		physx::PxShape* shape_ = nullptr;
		physx::PxRigidStatic* body_ = nullptr;

	};

}
