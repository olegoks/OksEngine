#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Render\OksEngine.Render.Subsystem.hpp>
#include <Resources\OksEngine.Resource.Subsystem.hpp>
#include <Resources/OksEngine.Resources.Components.hpp>

namespace OksEngine {

	ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr } {

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderMesh::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>().Include<Position>(), ECS::Entity::Filter{}.ExcludeAll() };
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

				auto model = Geom::ParseObjMtlModel(
					objName, { objResource->resourceData_.c_str(), objResource->resourceData_.size() },
					mtlName, { mtlResource->resourceData_.c_str(), mtlResource->resourceData_.size() }
				);
				for (auto& mesh : model.meshes_) {
					if (mesh.textureStorageTag_ != "") {
						if (!GetContext().GetTextureStorage()->IsExist(mesh.textureStorageTag_)) {
							const ECS::Entity::Id textureResourceEntityId = loadMeshRequest->resourceEntityId_[mesh.textureStorageTag_];
							auto* textureResource = world->GetComponent<Resource>(textureResourceEntityId);
							Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
							const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(mesh.textureStorageTag_, std::move(texture));

						}
						mesh.textureStorageId_ = GetContext().GetTextureStorage()->GetIdByTag(mesh.textureStorageTag_);
					}
				}

				auto* geomFile = world->GetComponent<GeometryFile>(entityId);
				const Geom::Model2::Id modelId = GetContext().GetGeomStorage()->Add(geomFile->mesh_["Name"].as<std::string>(), std::move(model));
				world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), modelId);
			}
		}
	}

	void RenderMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
		auto* meshComponent = world->GetComponent<Mesh>(entityId);
		if (meshComponent->driverModelId_ != Common::Limits<Common::Index>::Max()) return;
		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		auto& model = GetContext().GetGeomStorage()->Get(meshComponent->modelId_);
		for (const Geom::Mesh& mesh : model.meshes_) {
			if (mesh.textureStorageId_.IsInvalid()) continue;
			//TODO render mesh without textures
			auto& texture = GetContext().GetTextureStorage()->Get(mesh.textureStorageId_);
			std::vector<RAL::Color4b> texturePixels{ texture.GetPixels<RAL::Color4b>(), texture.GetPixels<RAL::Color4b>() + texture.GetPixelsNumber() };
			RAL::Texture::CreateInfo textureCreateInfo{
				.name_ = mesh.textureStorageTag_,
				.pixels_ = texturePixels,
				.size_ = texture.GetSize()
			};
			RAL::Texture::Id driverTextureId = driver->CreateTexture(textureCreateInfo);
			auto vertices = Geometry::GetVertexCloud3fnt(mesh);
			glm::mat4 localToWorld = position->GetTranslateMat();
			meshComponent->driverModelId_ = driver->DrawIndexed(
				localToWorld,
				(const RAL::Vertex3fnt*)vertices.GetData(),
				vertices.GetVerticesNumber(),
				(const RAL::Index16*)mesh.indices_.GetData(),
				mesh.indices_.GetIndicesNumber(), driverTextureId);
			driver->SetModelMatrix(meshComponent->driverModelId_, glm::mat4{ 1 }/*localToWorld*/);
		}

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
		OS::AssertMessage(meshType != LoadMeshRequest::Type::Undefined, { "Maybe %s contains incorrect data!", geomFile->geomName_.c_str() });
		world->CreateComponent<LoadMeshRequest>(entityId, resourceEntityId_, meshType);

	}

}