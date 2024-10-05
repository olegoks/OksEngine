
#include <PhysX.Shape.hpp>
#include <Geometry.Shapes.hpp>
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

		Geom::Box< Geom::Index16> box{ 5 };
		Geom::Vertex3f vertices[3] = {
			{ 0.f, 0.f, 0.f },
			{ 1.f, 0.f, 0.f },
			{ 0.f, 1.f, 0.f }
		};
		Geom::Index16 indices[3] = { 0, 1, 2 };
		physx::PxTriangleMeshDesc meshDesc{};
		{
			meshDesc.points.count = /*box.GetVerticesNumber();*//*static_cast<physx::PxU32>(box.GetVerticesNumber());*/static_cast<physx::PxU32>(createInfo.palCreateInfo_.vertices_.GetVerticesNumber());
			meshDesc.points.stride = sizeof(Geom::Vertex3f);
			OS::Assert(sizeof(physx::PxVec3) == sizeof(Geom::Vertex3f));
			meshDesc.points.data = /*box.GetVertices().GetData();*//*box.GetVertices().GetData();*/createInfo.palCreateInfo_.vertices_.GetData();

			meshDesc.triangles.count = /*box.GetIndices().GetIndicesNumber() /3 ;*/static_cast<physx::PxU32>(createInfo.palCreateInfo_.indices_.GetIndicesNumber()) / 3;
			meshDesc.triangles.stride = 3 * sizeof(physx::PxU16);
			meshDesc.triangles.data = /*box.GetIndices().GetData(); */ createInfo.palCreateInfo_.indices_.GetData();

			meshDesc.flags |= (physx::PxMeshFlag::e16_BIT_INDICES/* | physx::PxMeshFlag::eFLIPNORMALS*/);
		}
		OS::Assert(meshDesc.isValid());

		//physx::PxConvexMeshDesc convexMesh{};
		//{
		//	convexMesh.points.count = box.GetVerticesNumber();/*static_cast<physx::PxU32>(box.GetVerticesNumber());*///static_cast<physx::PxU32>(createInfo.palCreateInfo_.vertices_.GetVerticesNumber());
		//	convexMesh.points.stride = sizeof(Geom::Vertex3f);
		//	OS::Assert(sizeof(physx::PxVec3) == sizeof(Geom::Vertex3f));
		//	convexMesh.points.data = box.GetVertices().GetData();/*box.GetVertices().GetData();*///createInfo.palCreateInfo_.vertices_.GetData();

		//	convexMesh.polygons.count = box.GetIndices().GetIndicesNumber() / 3;//1;*//*static_cast<physx::PxU32>(box.GetIndicesNumber()) / 3;*/static_cast<physx::PxU32>(createInfo.palCreateInfo_.indices_.GetIndicesNumber()) / 3;
		//	convexMesh.polygons.stride = 3 * sizeof(physx::PxU16);
		//	convexMesh.polygons.data = box.GetIndices().GetData(); //indices;// createInfo.palCreateInfo_.indices_.GetData();//
		//	convexMesh.indices.data = box.GetIndices().GetData();
		//	convexMesh.indices.count = box.GetIndices().GetIndicesNumber();
		//	convexMesh.indices.stride = sizeof(physx::PxU16);
		//	convexMesh.flags |= (physx::PxConvexFlag::e16_BIT_INDICES/* | physx::PxMeshFlag::eFLIPNORMALS*/);
		//}
		//OS::Assert(convexMesh.isValid());

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);
		{
			// disable mesh cleaning - perform mesh validation on development configurations
			//params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			// disable edge precompute, edges are set for each triangle, slows contact generation
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		}
#ifdef _DEBUG
		// mesh should be validated before cooked without the mesh cleaning
		//bool res = PxValidateTriangleMesh(params, meshDesc);
		//OS::AssertMessage(res, "Attempt to use invalid mesh to create static rigid body.");
#endif
		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;
		//physx::PxConvexMeshCookingResult::Enum result;
		bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
		OS::Assert(status);

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		physx::PxTriangleMesh* triangleMesh = createInfo.physics_->createTriangleMesh(readBuffer);
		
		physx::PxShape* pxShape = createInfo.physics_->createShape(/*physx::PxBoxGeometry(1, 1, 1)*/physx::PxTriangleMeshGeometry{ triangleMesh }, *material);

		OS::AssertMessage(pxShape != nullptr,
			"Error while creating physx shape.");

		//physx::PxShape* shape = createInfo.physics_->createShape(physx::PxBoxGeometry(1, 1, 1), *material);

		material_ = material;
		shape_ = pxShape;
	}

}