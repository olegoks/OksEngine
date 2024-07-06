#pragma once 

#include <PAL.Shape.hpp>

#include <PhysX.Common.hpp>

namespace PhysX {


	class Shape : public PAL::Shape {
	public:

		struct CreateInfoBox {
			PAL::Shape::CreateInfoBox palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		struct CreateInfoCapsule {
			PAL::Shape::CreateInfoCapsule palCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		Shape(const CreateInfoBox& createInfo);
		Shape(const CreateInfoCapsule& createInfo);

		[[nodiscard]]
		physx::PxShape* GetPxShape() noexcept {
			return shape_;
		}

	private:

		[[nodiscard]]
		static physx::PxMaterial* CreateMaterial(
			physx::PxPhysics* physics,
			const PAL::Shape::Material& materialInfo);

	private:
		physx::PxShape* shape_ = nullptr;
		physx::PxMaterial* material_ = nullptr;
	};
}