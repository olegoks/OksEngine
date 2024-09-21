#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Render\OksEngine.Render.Subsystem.hpp>
#include <Resources\OksEngine.Resource.Subsystem.hpp>
#include <Resources/OksEngine.Resources.Components.hpp>
#include <Render/OksEngine.GeometryStorage.hpp>

namespace OksEngine {

	ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr } {

	}


	void LoadGeometryDescriptionFile::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* request = world->GetComponent<LoadGeometryDescriptionFileRequest>(entityId);
		using namespace std::string_literals;
		const std::string& geomFilePath = "Root\\"s + request->geomFileName_;
		if (request->resourceTaskEntityId_.IsInvalid()) {
			request->resourceTaskEntityId_ = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(request->resourceTaskEntityId_, request->geomFileName_);
		}
		else {
			if (world->IsComponentExist<Resource>(request->resourceTaskEntityId_)) {
				const auto* resource = world->GetComponent<Resource>(request->resourceTaskEntityId_);
				world->CreateComponent<GeometryFile>(entityId, request->geomFileName_, resource->resourceData_);
			}
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> LoadGeometryDescriptionFile::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<ImmutableRenderGeometry>()
			.Exclude<GeometryFile>()
			.Include<LoadGeometryDescriptionFileRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	void LoadMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* loadMeshRequest = world->GetComponent<LoadMeshRequest>(entityId);
		bool allResourcesLoaded = true;
		for (auto [resourceName, resourceEntityId] : loadMeshRequest->resourceEntityId_) {
			if (!world->IsComponentExist<Resource>(resourceEntityId)) {
				allResourcesLoaded = false;
			}
		}
		if (allResourcesLoaded) {
			LoadMeshRequest::Type meshType = loadMeshRequest->type_;
			if (meshType == LoadMeshRequest::Type::OBJ_MTL_TEXTURES) {

				ECS::Entity::Id objEntityId;
				std::string objName;
				ECS::Entity::Id mtlEntityId;
				std::string mtlName;
				for (auto& [name, entityId] : loadMeshRequest->resourceEntityId_) {
					if (name.find(".obj") != name.npos) {
						objName = name;
						objEntityId = entityId;
					}
					if (name.find(".mtl") != name.npos) {
						mtlName = name;
						mtlEntityId = entityId;
					}
				}
				OS::Assert(!objEntityId.IsInvalid() && !mtlEntityId.IsInvalid());
				auto* objResource = world->GetComponent<Resource>(objEntityId);
				auto* mtlResource = world->GetComponent<Resource>(mtlEntityId);
				OS::LogInfo("render", { "Processing model file {}...", objName.c_str() });
				auto model = Geom::ParseObjMtlModelBaked(
					objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() },
					mtlName, { mtlResource->resourceData_.c_str(), mtlResource->resourceData_.size() }
				);
				OS::LogInfo("render", { "File {} processed.", objName.c_str() });
				//auto model = Geom::ParseFbxModelBaked(
				//	objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() }
				//);
				auto modelStorage = GetContext().GetModelStorage();
				ModelStorage::Model::Id modelId = modelStorage->CreateModel(loadMeshRequest->name_);
				ModelStorage::Model& storageModel = modelStorage->GetModel(modelId);
				for (auto& mesh : model.meshes_) {
					ModelStorage::Model::MeshId storageMeshId = storageModel.AddMesh();
					ModelStorage::Model::Mesh& storageMesh = storageModel.GetMesh(storageMeshId);
					if (mesh.textureName_ != "") {
						if (!GetContext().GetTextureStorage()->IsExist(mesh.textureName_)) {
							const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureName_];
							auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
							Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
							const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureName_, std::move(texture));
							storageMesh.textureStorageId_ = textureId;
						}
						storageMesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureName_);
					}
					Geom::Mesh::Id meshId = GetContext().GetMeshStorage()->Add(mesh.name_, std::move(mesh));
					storageMesh.meshId_ = meshId;
				}

				auto* geomFile = world->GetComponent<GeometryFile>(entityId);
				world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), modelId);
			}
		}
	}

	void CreateDriverModel::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
		auto* meshComponent = world->GetComponent<Mesh>(entityId);
		if (meshComponent->driverModelId_ != Common::Limits<Common::Index>::Max()) return;
		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		auto& model = GetContext().GetModelStorage()->GetModel(meshComponent->modelId_);


		auto* driverCamera = world->GetComponent<DriverCamera>(secondEntityId);

		std::map<Geom::Texture::Id, RAL::Texture::Id> createdTextures;
		std::vector<DriverModel::Mesh> driverMeshs;

		model.ForEachMesh([&](ModelStorage::Model::Mesh& mesh) {
			if (mesh.textureStorageId_.IsInvalid()) return true;

			std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

			//CAMERA BINDING
			{
				RAL::Driver::ShaderBinding::Data cameraBinding{
					.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
					.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
					.uniformBufferId_ = driverCamera->matricesBuffer_
				};
				shaderBindings.push_back(cameraBinding);
			}

			//TRANSFORM BINDING
			{
				struct Transform {
					glm::mat4 model_;
				};

				Transform transform{ position->GetMat() };
				RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
					.size_ = sizeof(Transform),
					.type_ = RAL::Driver::UniformBuffer::Type::Const
				};
				RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);

				RAL::Driver::ShaderBinding::Data transformBinding{
					.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
					.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
					.uniformBufferId_ = ubId
				};
				shaderBindings.push_back(transformBinding);
			}

			//TEXTURE BINDING
			{
				RAL::Texture::Id driverTextureId;
				if (createdTextures.contains(mesh.textureStorageId_)) {
					const RAL::Texture::Id textureId = createdTextures[mesh.textureStorageId_];
					driverTextureId = textureId;
				} else {
					auto& texture = GetContext().GetTextureStorage()->Get(mesh.textureStorageId_);
					std::vector<RAL::Color4b> texturePixels{ texture.GetPixels<RAL::Color4b>(), texture.GetPixels<RAL::Color4b>() + texture.GetPixelsNumber() };
					RAL::Texture::CreateInfo textureCreateInfo{
						.name_ = "",
						.pixels_ = texturePixels,
						.size_ = texture.GetSize()
					};
					RAL::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);
					driverTextureId = textureId;
					createdTextures[mesh.textureStorageId_] = textureId;
				}

				OS::AssertMessage(!driverTextureId.IsInvalid(), "");

				RAL::Driver::ShaderBinding::Data textureBinding{
					.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
					.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
					.textureId_ = driverTextureId
				};

				shaderBindings.push_back(textureBinding);
			}

			const Geom::Mesh& geomMesh = GetContext().GetMeshStorage()->Get(mesh.meshId_);
			auto vertices = Geometry::GetVertexCloud3fnt(geomMesh);
			//glm::mat4 localToWorld = position->GetTranslateMat();

			Common::Id driverModelId = driver->DrawMesh(
				"Textured Pipeline",
				(const RAL::Vertex3fnt*)vertices.GetData(),
				vertices.GetVerticesNumber(),
				RAL::Driver::VertexType::VF3_NF3_TF2,
				(const RAL::Index16*)geomMesh.indices_.GetData(),
				geomMesh.indices_.GetIndicesNumber(),
				RAL::Driver::IndexType::UI16,
				shaderBindings
			);

			DriverModel::Mesh driverMesh{
				.id_ = driverModelId,
				.shaderBindings_ = shaderBindings
			};

			driverMeshs.push_back(driverMesh);

			return true;

		});

		world->CreateComponent<DriverModel>(entityId, driverMeshs);

	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverModel::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<Mesh>()
			.Exclude<DriverModel>(), ECS::Entity::Filter{}.Include<DriverCamera>() };
	}

	void RenderMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* driverModel = world->GetComponent<DriverModel>(entityId);



			//for (const ModelStorage::Model::Mesh::Id& mesh : model.meshs_) {
			//	if (mesh.textureName_.IsInvalid()) continue;
			//	//TODO render mesh without textures
			//	auto& texture = GetContext().GetTextureStorage()->Get(mesh.textureStorageId_);
			//	std::vector<RAL::Color4b> texturePixels{ texture.GetPixels<RAL::Color4b>(), texture.GetPixels<RAL::Color4b>() + texture.GetPixelsNumber() };
			//	RAL::Texture::CreateInfo textureCreateInfo{
			//		.name_ = mesh.textureStorageTag_,
			//		.pixels_ = texturePixels,
			//		.size_ = texture.GetSize()
			//	};
			//	RAL::Texture::Id driverTextureId = driver->CreateTexture(textureCreateInfo);
			//	auto vertices = Geometry::GetVertexCloud3fnt(mesh);
			//	glm::mat4 localToWorld = position->GetTranslateMat();
			//	meshComponent->driverModelId_ = driver->DrawIndexed(
			//		localToWorld,
			//		(const RAL::Vertex3fnt*)vertices.GetData(),
			//		vertices.GetVerticesNumber(),
			//		(const RAL::Index16*)mesh.indices_.GetData(),
			//		mesh.indices_.GetIndicesNumber(), driverTextureId);
			//	driver->SetModelMatrix(meshComponent->driverModelId_, glm::mat4{ 1 }/*localToWorld*/);
			//}

	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderMesh::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>().Include<Position>().Include<DriverModel>(), ECS::Entity::Filter{}.ExcludeAll()  };
	}


	void CreateLoadMeshRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* geomFile = world->GetComponent<GeometryFile>(entityId);
		std::map<std::string, ECS::Entity::Id> resourceEntityId_;
		LoadMeshRequest::Type meshType = LoadMeshRequest::Type::Undefined;
		if (geomFile->mesh_["Obj"]) {

			const std::string objName = geomFile->mesh_["Obj"].as<std::string>();
			const ECS::Entity::Id loadObjResourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(loadObjResourceEntityId, objName);
			resourceEntityId_[objName] = loadObjResourceEntityId;
			meshType = LoadMeshRequest::Type::OBJ;

			if (geomFile->mesh_["Mtl"]) {
				const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
				const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
				world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId, mtlName);
				resourceEntityId_[mtlName] = loadMtlResourceEntityId;

				meshType = LoadMeshRequest::Type::OBJ_MTL;

				if (geomFile->mesh_["Textures"] && geomFile->mesh_["Textures"].size() != 0) {
					for (Common::Index i = 0; i < geomFile->mesh_["Textures"].size(); i++) {
						const std::string textureName = geomFile->mesh_["Textures"][i].as<std::string>();
						const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();
						world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId, textureName);
						resourceEntityId_[textureName] = loadTextureResourceEntityId;
					}
					meshType = LoadMeshRequest::Type::OBJ_MTL_TEXTURES;
				}
			}
		}
		if (geomFile->mesh_["Fbx"]) {

			const std::string fbxName = geomFile->mesh_["Fbx"].as<std::string>();
			const ECS::Entity::Id loadFbxResourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(loadFbxResourceEntityId, fbxName);
			resourceEntityId_[fbxName] = loadFbxResourceEntityId;
			meshType = LoadMeshRequest::Type::FBX_TEXTURES;

			if (geomFile->mesh_["Mtl"]) {
				const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
				const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
				world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId, mtlName);
				resourceEntityId_[mtlName] = loadMtlResourceEntityId;

				meshType = LoadMeshRequest::Type::OBJ_MTL;

				if (geomFile->mesh_["Textures"] && geomFile->mesh_["Textures"].size() != 0) {
					for (Common::Index i = 0; i < geomFile->mesh_["Textures"].size(); i++) {
						const std::string textureName = geomFile->mesh_["Textures"][i].as<std::string>();
						const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();
						world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId, textureName);
						resourceEntityId_[textureName] = loadTextureResourceEntityId;
					}
					meshType = LoadMeshRequest::Type::OBJ_MTL_TEXTURES;
				}
			}
		}
		OS::AssertMessage(meshType != LoadMeshRequest::Type::Undefined, { "Maybe %s contains incorrect data!", geomFile->geomName_.c_str() });
		const std::string name = geomFile->mesh_["Name"].as<std::string>();
		world->CreateComponent<LoadMeshRequest>(entityId, name, resourceEntityId_, meshType);

	}



	void MapMeshTransform::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position>(entityId);
		auto* driverModel = world->GetComponent<DriverModel>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		struct Transform {
			alignas(16) glm::mat4 model_;
		};

		Transform transform{ position->GetMat() };

		for (DriverModel::Mesh& mesh : driverModel->driverMeshs_) {
			auto& transformBinding = mesh.shaderBindings_[1];
			driver->FillUniformBuffer(transformBinding.uniformBufferId_, &transform);
		}

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> MapMeshTransform::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
		.Include<Position>()
		.Include<DriverModel>(), ECS::Entity::Filter{}.ExcludeAll()};

	}


}