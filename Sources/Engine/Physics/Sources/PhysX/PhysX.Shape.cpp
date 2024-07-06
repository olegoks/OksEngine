
#include <PhysX.Shape.hpp>

#include <OS.Assert.hpp>
namespace PhysX {


	Shape::Shape(const CreateInfoBox& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = createInfo.physics_->createMaterial(0.5f, 0.5f, 0.6f);

		physx::PxShape* shape = createInfo.physics_->createShape(
			physx::PxBoxGeometry(
				createInfo.palCreateInfo_.halfExtentX_,
				createInfo.palCreateInfo_.halfExtentY_,
				createInfo.palCreateInfo_.halfExtentZ_),
			*material);
		OS::AssertMessage(shape != nullptr, "Error while creating physx shape.");
		shape_ = shape;

	}
	Shape::Shape(const CreateInfoCapsule& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


	}

}