
#include <PhysX.Shape.hpp>

#include <OS.Assert.hpp>
namespace PhysX {

	[[nodiscard]]
	physx::PxMaterial* Shape::CreateMaterial(
		physx::PxPhysics* physics,
		const PAL::Shape::Material& materialInfo) {
		physx::PxMaterial* material = physics->createMaterial(
			materialInfo.staticFriction_,
			materialInfo.dynamicFriction_,
			materialInfo.restitution_);
		OS::AssertMessage(material != nullptr, "Error while creating physx material.");
		return material;
	}

	Shape::Shape(const CreateInfoBox& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = CreateMaterial(
			createInfo.physics_,
			createInfo.palCreateInfo_.material_);

		physx::PxShape* shape = createInfo.physics_->createShape(
			physx::PxBoxGeometry(
				createInfo.palCreateInfo_.halfExtentX_,
				createInfo.palCreateInfo_.halfExtentY_,
				createInfo.palCreateInfo_.halfExtentZ_),
			*material);

		OS::AssertMessage(shape != nullptr,
			"Error while creating physx shape.");

		material_ = material;
		shape_ = shape;

	}
	Shape::Shape(const CreateInfoCapsule& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = CreateMaterial(
			createInfo.physics_,
			createInfo.palCreateInfo_.material_);

		physx::PxShape* shape = createInfo.physics_->createShape(
			physx::PxCapsuleGeometry(
				createInfo.palCreateInfo_.radius_,
				createInfo.palCreateInfo_.height_),
			*material);

		OS::AssertMessage(shape != nullptr,
			"Error while creating physx shape.");

		material_ = material;
		shape_ = shape;


	}

}