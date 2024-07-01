#pragma once 

#include <PAL.RigidBody.hpp>

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>

#include <Geometry.Shapes.hpp>

namespace PhysX {

	class RigidBody : public PAL::RigidBody {
	public:
		struct CreateInfo {
			PAL::RigidBody::CreateInfo palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
			
		};

		RigidBody(const CreateInfo& createInfo) : PAL::RigidBody{ createInfo.palCreateInfo_ }{

			physx::PxMaterial* material = createInfo.physics_->createMaterial(0.5f, 0.5f, 0.6f);
			Math::Vector3f translate = createInfo.palCreateInfo_.transform_.GetOrigin();
			physx::PxTransform t{ physx::PxVec3{ translate.GetX(), translate.GetY(), translate.GetZ() } };
			shape_ = createInfo.physics_->createShape(
				physx::PxBoxGeometry(
					createInfo.palCreateInfo_.halfExtentX_,
					createInfo.palCreateInfo_.halfExtentY_, 
					createInfo.palCreateInfo_.halfExtentX_),
				*material);
			body_ = createInfo.physics_->createRigidDynamic(t);

			body_->attachShape(*shape_);
			physx::PxRigidBodyExt::updateMassAndInertia(*body_, 10.f);
		}

		[[nodiscard]]
		physx::PxRigidDynamic* GetBody() {
			OS::Assert(body_ != nullptr);
			return body_;
		}

		virtual const Math::Matrix4x4f GetTransform() override {

			const physx::PxTransform transform = GetBody()->getGlobalPose();
			return Math::Matrix4x4f::GetTranslate({ transform.p.x, transform.p.y, transform.p.z });
		}

	private:
		physx::PxMaterial* material_ = nullptr;
		physx::PxShape* shape_ = nullptr;
		physx::PxRigidDynamic* body_ = nullptr;

	};

}