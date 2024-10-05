
#include <Render/OksEngine.CreateModel.hpp>
#include <Render/OksEngine.Obj.hpp>
#include <Render/OksEngine.Mtl.hpp>
#include <Common/OksEngine.ChildEntity.hpp>
#include <Render/OksEngine.Mesh.hpp>
#include <Render/OksEngine.Model.hpp>
#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.Texture.hpp>
#include <Render/OksEngine.ModelEntity.hpp>


#include <Geometry.Model.hpp>

namespace OksEngine {

	void CreateModelEntityFromObjMtl::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* obj = world->GetComponent<Obj>(entityId);
		auto* mtl = world->GetComponent<Mtl>(entityId);
		auto* geomFile = world->GetComponent<GeometryFile>(entityId);

		ECS::Entity::Id modelEntityid = world->CreateEntity();
		std::vector<ECS::Entity::Id> meshsEntitiesIds;

		Geom::Model2 model2 = Geom::ParseObjMtlModelBaked(obj->name_, obj->data_, mtl->name_, mtl->data_);

		//auto modelStorage = GetContext().GetModelStorage();
		//ModelStorage::Model::Id modelId = modelStorage->CreateModel(loadMeshRequest->name_);
		//ModelStorage::Model& storageModel = modelStorage->GetModel(modelId);
		for (auto& mesh : model2.meshes_) {
		/*	ModelStorage::Model::MeshId storageMeshId = storageModel.AddMesh();
			ModelStorage::Model::Mesh& storageMesh = storageModel.GetMesh(storageMeshId);*/
			ECS::Entity::Id meshEntityId = world->CreateEntity();
			meshsEntitiesIds.push_back(meshEntityId);
			if (mesh.textureName_ != "") {
				world->CreateComponent<Texture>(meshEntityId, mesh.textureName_);
			}
			
			OS::Assert(mesh.vertices_.GetVerticesNumber() != 0);
			world->CreateComponent<Vertices>(meshEntityId, mesh.vertices_);

			OS::Assert(mesh.indices_.GetIndicesNumber() != 0);
			world->CreateComponent<Indices>(meshEntityId, mesh.indices_);

			if (mesh.colors_.GetSize() != 0) {
				world->CreateComponent<Colors>(meshEntityId, mesh.colors_);
			}

			if (mesh.uvs_.GetSize() != 0) {
				world->CreateComponent<UVs>(meshEntityId, mesh.uvs_);
			}


		/*	if (mesh.textureName_ != "") {
				if (!GetContext().GetTextureStorage()->IsExist(mesh.textureName_)) {
					const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureName_];
					auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
					Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
					const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureName_, std::move(texture));
					storageMesh.textureStorageId_ = textureId;
				}
				storageMesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureName_);
			}*/
			//Geom::Mesh::Id meshId = GetContext().GetMeshStorage()->Add(mesh.name_, std::move(mesh));
			//storageMesh.meshId_ = meshId;
		}


		world->CreateComponent<ChildEntities>(modelEntityid, meshsEntitiesIds);


		//auto* geomFile = world->GetComponent<GeometryFile>(entityId);
		//world->CreateComponent<Mesh>(entityId, modelId); */

		world->CreateComponent<ModelEntity>(entityId, modelEntityid);

		/*auto* loadMeshRequest = world->GetComponent<LoadMeshRequest>(entityId);
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

		auto* geomFile = world->GetComponent<GeometryFile>(entityId);
		world->CreateComponent<Mesh>(entityId, modelId);*/
	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromObjMtl::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Obj>().Include<Mtl>().Exclude<ModelEntity>(), ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateModelEntityFromObjMtl::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromObjMtl>().GetId();
	}
}