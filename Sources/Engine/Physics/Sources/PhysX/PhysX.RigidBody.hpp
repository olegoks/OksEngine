#pragma once 

#include <PAL.RigidBody.hpp>

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>

#include <Geometry.Shapes.hpp>

#include <PhysX.Common.hpp>
#include <PhysX.Shape.hpp>

namespace PhysX {

	class RigidBody : public PAL::RigidBody {
	public:
		struct CreateInfo {
			PAL::RigidBody::CreateInfo palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
			
		};

		RigidBody(const CreateInfo& createInfo) : PAL::RigidBody{ createInfo.palCreateInfo_ }{

			physx::PxMat44 pxMatrix = convertToPxMat44(createInfo.palCreateInfo_.transform_);
			physx::PxTransform t{ pxMatrix };
			auto shape = std::static_pointer_cast<PhysX::Shape>(createInfo.palCreateInfo_.shape_);
			body_ = createInfo.physics_->createRigidDynamic(t);
			OS::AssertMessage(body_ != nullptr,
				"Error while creating rigid body.");
			body_->attachShape(*shape->GetPxShape());
			body_->setLinearDamping(createInfo.palCreateInfo_.linearDamping_);
			physx::PxRigidBodyExt::updateMassAndInertia(*body_, createInfo.palCreateInfo_.mass_);
		}

		[[nodiscard]]
		physx::PxRigidDynamic* GetBody() {
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
			//PxTransform globalPose(= actor->getGlobalPose();
			//­
			//	// Устанавливаем вертикальный вектор вверх (PxVec3(0, 1, 0) чтобы избежать наклона
			//	physx::PxVec3 up = PxVec3(0, 1, 0);
			//globalPose.q = PxQuat(up, PxVec3(0, 1, 0));
			//­
			//	// Обновляем позицию и сохраняем ориентацию
			//	globalPose.p += displacement;
			//actor->setGlobalPose(globalPose);
			GetBody()->setGlobalPose(pxTransform);
		}


		virtual void ApplyForce(const glm::vec3& direction, float force) override {
			const glm::vec3 directionalForce = glm::normalize(direction) * force;
			GetBody()->addForce({ directionalForce.x, directionalForce.y, directionalForce.z });
		}


	private:
		physx::PxMaterial* material_ = nullptr;
		physx::PxShape* shape_ = nullptr;
		physx::PxRigidDynamic* body_ = nullptr;

	};

}
