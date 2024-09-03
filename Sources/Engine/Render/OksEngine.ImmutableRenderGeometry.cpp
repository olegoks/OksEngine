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
		}
		/*const auto* geomFile = world->GetComponent<GeometryFile>(entityId);
		if (geomFile->mesh_["Obj"]) {
			const std::string objName = geomFile->mesh_["Obj"].as<std::string>();
			using namespace std::string_literals;
			const std::string& objFilePath = "Root\\"s + objName;
			auto objTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, objFilePath);
			ResourceSubsystem::Resource objResource = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, objTaskId);

			const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
			using namespace std::string_literals;
			const std::string& mtlFilePath = "Root\\"s + mtlName;
			auto mtlTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, mtlFilePath);
			ResourceSubsystem::Resource mtlResource = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, mtlTaskId);

			auto mesh = Geom::ParseObjMtlModel(
				objName, { objResource.GetData<char>(), objResource.GetSize() },
				mtlName, { mtlResource.GetData<char>(), mtlResource.GetSize() }
			);

			for (Common::Index i = 0; i < geomFile->mesh_["Textures"].size(); i++) {
				const std::string textureName = geomFile->mesh_["Textures"][i].as<std::string>();
				using namespace std::string_literals;
				const std::string& textureFilePath = "Root\\"s + textureName;
				auto textureTaskId = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, textureFilePath);
				ResourceSubsystem::Resource textureResource = GetContext().GetResourceSubsystem()->GetResource(Subsystem::Type::Engine, textureTaskId);
				Geom::Texture texture = Geom::CreateTexture(textureResource.GetData<char>(), textureResource.GetSize());
				const Geom::Texture::Id textureId = GetContext().GetTextureStorage()->Add(textureName, std::move(texture));
				mesh.textureStorageTag_ = textureName;
				mesh.textureStorageId_ = textureId;
			}

			const Geom::Mesh::Id meshId = GetContext().GetGeomStorage()->Add(geomFile->mesh_["Name"].as<std::string>(), std::move(mesh));
			world->CreateComponent<Mesh>(entityId, geomFile->mesh_["Name"].as<std::string>(), meshId);
		}*/
	}

	void RenderMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
		auto* meshComponent = world->GetComponent<Mesh>(entityId);
		if (meshComponent->driverModelId_ != Common::Limits<Common::Index>::Max()) return;
		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		auto& mesh = GetContext().GetGeomStorage()->Get(meshComponent->meshId_);
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