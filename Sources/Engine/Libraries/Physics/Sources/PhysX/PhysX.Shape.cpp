
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
		ASSERT_FMSG(material != nullptr, "Error while creating physx material.");
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

		shape->setFlag(physx::PxShapeFlag::Enum::eVISUALIZATION, true);
		ASSERT_FMSG(shape != nullptr,
			"Error while creating physx shape.");

		material_ = material;
		shape_ = shape;

	}

	//Shape::Shape(const CreateInfoCylinder& createInfo) :
	//	PAL::Shape{ createInfo.palCreateInfo_ }{

	//	physx::PxMaterial* material = CreateMaterial(
	//		createInfo.physics_,
	//		createInfo.palCreateInfo_.material_);

	//	physx::PxShape* shape = createInfo.physics_->createShape(
	//		physx::PxCapsuleGeometry(
	//			createInfo.palCreateInfo_.radius_,
	//			createInfo.palCreateInfo_.height_ / 2),
	//		*material);

	//	ASSERT_FMSG(shape != nullptr,
	//		"Error while creating physx shape.");

	//	material_ = material;
	//	shape_ = shape;

	//}

	Shape::Shape(const CreateInfoCapsule& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = CreateMaterial(
			createInfo.physics_,
			createInfo.palCreateInfo_.material_);

		physx::PxShape* shape = createInfo.physics_->createShape(
			physx::PxCapsuleGeometry(
				createInfo.palCreateInfo_.radius_,
				createInfo.palCreateInfo_.height_ / 2),
			*material);


		//90 dg around Z
		physx::PxQuat rotation(physx::PxPi / 2, physx::PxVec3(0.0f, 0.0f, 1.0f));
		physx::PxTransform localTransform = shape->getLocalPose();

		localTransform.p = physx::PxVec3{ 0, createInfo.palCreateInfo_.height_ / 2, 0 };
		localTransform.q = rotation;

		shape->setLocalPose(localTransform);

		ASSERT_FMSG(shape != nullptr,
			"Error while creating physx shape.");

		shape->setFlag(physx::PxShapeFlag::Enum::eVISUALIZATION, true);

		material_ = material;
		shape_ = shape;
	}

	Shape::Shape(const CreateInfoMesh& createInfo) :
		PAL::Shape{ createInfo.palCreateInfo_ }{


		physx::PxMaterial* material = CreateMaterial(
			createInfo.physics_,
			createInfo.palCreateInfo_.material_);

		if (!createInfo.palCreateInfo_.convexGeometry_) {

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
			
			
					physx::PxTolerancesScale scale;
					physx::PxCookingParams params(scale);
					{
						// disable mesh cleaning - perform mesh validation on development configurations
						//params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
						// disable edge precompute, edges are set for each triangle, slows contact generation
						//params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
					}
			#ifdef _DEBUG
					// mesh should be validated before cooked without the mesh cleaning
					//bool res = PxValidateTriangleMesh(params, meshDesc);
					//ASSERT_FMSG(res, "Attempt to use invalid mesh to create static rigid body.");
			#endif
					physx::PxDefaultMemoryOutputStream writeBuffer;
					physx::PxTriangleMeshCookingResult::Enum result;
					//physx::PxConvexMeshCookingResult::Enum result;
					bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
					OS::Assert(status);
			
					physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
					physx::PxTriangleMesh* triangleMesh = createInfo.physics_->createTriangleMesh(readBuffer);
			
					physx::PxShape* pxShape = createInfo.physics_->createShape(/*physx::PxBoxGeometry(1, 1, 1)*/physx::PxTriangleMeshGeometry{ triangleMesh }, *material);
					shape_ = pxShape;
			
					ASSERT_FMSG(pxShape != nullptr, "Error while creating physx shape.");
					material_ = material;
		}
		else {

			//Workable convex mesh creation.
			using namespace physx;
			PxConvexMeshDesc convexDesc;
			{
				convexDesc.points.count = createInfo.palCreateInfo_.vertices_.GetVerticesNumber();
				convexDesc.points.stride = sizeof(Geom::Vertex3f);
				STATIC_ASSERT_MSG(sizeof(Geom::Vertex3f) == sizeof(decltype(createInfo.palCreateInfo_.vertices_[0])), "");
				convexDesc.points.data = createInfo.palCreateInfo_.vertices_.GetData();
				convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX/* | PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES*/;
				convexDesc.vertexLimit = 255;
			}

			physx::PxCookingParams params(physx::PxTolerancesScale{});
			{
				// disable mesh cleaning - perform mesh validation on development configurations
				params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
				// disable edge precompute, edges are set for each triangle, slows contact generation
				params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			}
			PxDefaultMemoryOutputStream writeBuffer;
			physx::PxConvexMeshCookingResult::Enum cookingResult;
			bool cookResult = PxCookConvexMesh(params, convexDesc, writeBuffer, &cookingResult);
			ASSERT_MSG(cookResult, "Invalid mesh data to create phys shape.");

			physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
			physx::PxConvexMesh* convexMesh = createInfo.physics_->createConvexMesh(readBuffer);
			PxConvexMeshGeometry convexGeometry{ convexMesh };
			PxTransform localTransform(PxVec3(0, 0, 0));
			physx::PxShape* pxShape = createInfo.physics_->createShape(convexGeometry, *material);
			pxShape->setLocalPose(localTransform);
			pxShape->setFlag(physx::PxShapeFlag::Enum::eVISUALIZATION, true);
			physx::PxReal mass;
			physx::PxMat33 inertia;
			physx::PxVec3 massCenter;

			convexMesh->getMassInformation(mass, inertia, massCenter);

			ASSERT_FMSG(pxShape != nullptr, "Error while creating physx shape.");
			material_ = material;
			shape_ = pxShape;
		}

	}

}