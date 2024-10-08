
#include <Render/OksEngine.LoadMesh.hpp>

#include <Render/OksEngine.LoadMeshRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Geometry.Model.hpp>
#include <Render/OksEngine.ModelStorage.hpp>
#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.Mesh.hpp>

namespace OksEngine {
	void LoadMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* loadMeshRequest = world->GetComponent<LoadMeshRequest>(entityId);
		bool allResourcesLoaded = true;
		for (auto [resourceName, resourceEntityId] : loadMeshRequest->resourceEntityId_) {
			if (!world->IsComponentExist<Resource>(resourceEntityId)) {
				allResourcesLoaded = false;
			}
		}

		if (!allResourcesLoaded) {
			return;
		}

		Geom::Model2 model;

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
			model = Geom::ParseObjMtlModelBaked(
				objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() },
				mtlName, { mtlResource->resourceData_.c_str(), mtlResource->resourceData_.size() }
			);
			OS::LogInfo("render", { "File {} processed.", objName.c_str() });
			//auto model = Geom::ParseFbxModelBaked(
			//	objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() }
			////);
			//auto modelStorage = GetContext().GetModelStorage();
			//ModelStorage::Model::Id modelId = modelStorage->CreateModel(loadMeshRequest->name_);
			//ModelStorage::Model& storageModel = modelStorage->GetModel(modelId);
			//for (auto& mesh : model.meshes_) {
			//	ModelStorage::Model::MeshId storageMeshId = storageModel.AddMesh();
			//	ModelStorage::Model::Mesh& storageMesh = storageModel.GetMesh(storageMeshId);
			//	if (mesh.textureName_ != "") {
			//		if (!GetContext().GetTextureStorage()->IsExist(mesh.textureName_)) {
			//			const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureName_];
			//			auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
			//			Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
			//			const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureName_, std::move(texture));
			//			storageMesh.textureStorageId_ = textureId;
			//		}
			//		storageMesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureName_);
			//	}
			//	Geom::Mesh::Id meshId = GetContext().GetMeshStorage()->Add(mesh.name_, std::move(mesh));
			//	storageMesh.meshId_ = meshId;
			//}

			//auto* geomFile = world->GetComponent<GeometryFile>(entityId);
			//world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), modelId);
		}
		else if (meshType == LoadMeshRequest::Type::FBX) {

			ECS::Entity::Id fbxEntityId;
			std::string fbxName;
			for (auto& [name, entityId] : loadMeshRequest->resourceEntityId_) {
				if (name.find(".fbx") != name.npos) {
					fbxName = name;
					fbxEntityId = entityId;
				}
			}
			OS::Assert(!fbxEntityId.IsInvalid());
			auto* objResource = world->GetComponent<Resource>(fbxEntityId);
			OS::LogInfo("render", { "Processing model file {}...", fbxName.c_str() });
			model = Geom::ParseFbxModelBaked(
				fbxName,
				{
					objResource->resourceData_.c_str(),
					objResource->resourceData_.size()
				});
			OS::LogInfo("render", { "File {} processed.", fbxName.c_str() });
		}

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

		auto* geomFile = world->GetComponent<GeometryDescriptionFile>(entityId);
		world->CreateComponent<Mesh>(entityId, modelId);
	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> LoadMesh::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<LoadMeshRequest>().Include<GeometryDescriptionFile>().Exclude<Mesh>(), ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId LoadMesh::GetTypeId() const noexcept {
		return Common::TypeInfo<LoadMesh>().GetId();
	}
}