#pragma once 

#include <PAL.RigidBody.hpp>

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>

#include <Geometry.Shapes.hpp>

#include "PhysX.Common.hpp"

namespace PhysX {

	class RigidBody : public PAL::RigidBody {
	public:
		struct CreateInfo {
			PAL::RigidBody::CreateInfo palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
			
		};

		RigidBody(const CreateInfo& createInfo) : PAL::RigidBody{ createInfo.palCreateInfo_ }{

			physx::PxMaterial* material = createInfo.physics_->createMaterial(0.5f, 0.5f, 0.6f);
			
			physx::PxMat44 pxMatrix = convertToPxMat44(createInfo.palCreateInfo_.transform_);
			physx::PxTransform t{ pxMatrix };
			shape_ = createInfo.physics_->createShape(
				physx::PxBoxGeometry(
					createInfo.palCreateInfo_.halfExtentX_,
					createInfo.palCreateInfo_.halfExtentY_, 
					createInfo.palCreateInfo_.halfExtentZ_),
				*material);
			body_ = createInfo.physics_->createRigidDynamic(t);
			OS::AssertMessage(body_ != nullptr,
				"Error while creating rigid body.");
			body_->attachShape(*shape_);
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
			//glm::vec3 around{ 1.f, 0.0f, 0.0 } ;
			//glmMatrix = glm::rotate(glmMatrix, 90.f, around);
			//glmMatrix = glm::translate(glmMatrix, { transform.p.x, transform.p.y, transform.p.z });
			//Math::Matrix4x4f rotate;
			//rotate.GetColumn(0) = rotateGlm[0];
			return glmMatrix;
		}

	private:
		physx::PxMaterial* material_ = nullptr;
		physx::PxShape* shape_ = nullptr;
		physx::PxRigidDynamic* body_ = nullptr;

	};

}
