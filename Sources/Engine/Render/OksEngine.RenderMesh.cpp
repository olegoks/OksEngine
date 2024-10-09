
#include <Render/OksEngine.RenderMesh.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.DriverModel.hpp>
#include <Render/OksEngine.Mesh.hpp>

namespace OksEngine {


	//void RenderMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

	//	auto* driverModel = world->GetComponent<DriverModel>(entityId);



	//	//for (const ModelStorage::Model::Mesh::Id& mesh : model.meshs_) {
	//	//	if (mesh.textureName_.IsInvalid()) continue;
	//	//	//TODO render mesh without textures
	//	//	auto& texture = GetContext().GetTextureStorage()->Get(mesh.textureStorageId_);
	//	//	std::vector<RAL::Color4b> texturePixels{ texture.GetPixels<RAL::Color4b>(), texture.GetPixels<RAL::Color4b>() + texture.GetPixelsNumber() };
	//	//	RAL::Texture::CreateInfo textureCreateInfo{
	//	//		.name_ = mesh.textureStorageTag_,
	//	//		.pixels_ = texturePixels,
	//	//		.size_ = texture.GetSize()
	//	//	};
	//	//	RAL::Texture::Id driverTextureId = driver->CreateTexture(textureCreateInfo);
	//	//	auto vertices = Geometry::GetVertexCloud3fnt(mesh);
	//	//	glm::mat4 localToWorld = position->GetTranslateMat();
	//	//	meshComponent->driverModelId_ = driver->DrawIndexed(
	//	//		localToWorld,
	//	//		(const RAL::Vertex3fnt*)vertices.GetData(),
	//	//		vertices.GetVerticesNumber(),
	//	//		(const RAL::Index16*)mesh.indices_.GetData(),
	//	//		mesh.indices_.GetIndicesNumber(), driverTextureId);
	//	//	driver->SetModelMatrix(meshComponent->driverModelId_, glm::mat4{ 1 }/*localToWorld*/);
	//	//}

	//}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderMesh::GetFilter() const noexcept {
	//	return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>().Include<Position>().Include<DriverModel>(), ECS::Entity::Filter{}.ExcludeAll() };
	//}


	//Common::TypeId RenderMesh::GetTypeId() const noexcept {
	//	return Common::TypeInfo<RenderMesh>().GetId();
	//}

}