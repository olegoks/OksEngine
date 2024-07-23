
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

	Shape::Shape(const CreateInfoMesh& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = CreateMaterial(
			createInfo.physics_,
			createInfo.palCreateInfo_.material_);

		const auto& shapeGeom = createInfo.palCreateInfo_.shape_;

		physx::PxTriangleMeshDesc meshDesc;
		{
			meshDesc.points.count = static_cast<physx::PxU32>(shapeGeom.GetVerticesNumber());
			meshDesc.points.stride = sizeof(physx::PxVec3);
			OS::Assert(sizeof(physx::PxVec3) == sizeof(Geom::Vertex3f));
			meshDesc.points.data = shapeGeom.GetVertices().GetData();

			meshDesc.triangles.count = static_cast<physx::PxU32>(shapeGeom.GetIndicesNumber()) / 3;
			meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
			meshDesc.triangles.data = shapeGeom.GetIndices().GetData();
		}

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);
#ifdef _DEBUG
		// mesh should be validated before cooked without the mesh cleaning
		bool res = PxValidateTriangleMesh(params, meshDesc);
		PX_ASSERT(res);
#endif
		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;
		bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
		OS::Assert(status);

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		physx::PxTriangleMesh* triangleMesh = createInfo.physics_->createTriangleMesh(readBuffer);
		physx::PxShape* pxShape = createInfo.physics_->createShape(physx::PxTriangleMeshGeometry{ triangleMesh }, *material);

		OS::AssertMessage(pxShape != nullptr,
			"Error while creating physx shape.");

		material_ = material;
		shape_ = pxShape;
	}

}