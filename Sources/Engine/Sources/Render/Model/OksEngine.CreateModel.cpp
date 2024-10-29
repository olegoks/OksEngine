
#include <Render/Model/OksEngine.CreateModel.hpp>

#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

namespace OksEngine {

	void CreateModelEntityFromObjMtl::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		return;
		auto* position = world->GetComponent<Position>(entityId);
		auto* rotation = world->GetComponent<Rotation>(entityId);
		auto* objEntity = world->GetComponent<ObjEntity>(entityId);
		auto* mtlEntity = world->GetComponent<MtlEntity>(entityId);
		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);

		ECS::Entity::Id modelEntityid = world->CreateEntity();
		std::vector<ECS::Entity::Id> meshsEntitiesIds;

		auto* objName = world->GetComponent<Name>(objEntity->id_);
		auto* objData = world->GetComponent<BinaryData>(objEntity->id_);

		auto* mtlName = world->GetComponent<Name>(mtlEntity->id_);
		auto* mtlData = world->GetComponent<BinaryData>(mtlEntity->id_);

		Geom::Model2 model2 = Geom::ParseObjMtlModelBaked(
			objName->value_, 
			std::string{ objData->data_.data(), objData->data_.size() }, 
			mtlName->value_,
			std::string{ mtlData->data_.data(), mtlData->data_.size() } );

		for (auto& mesh : model2.meshes_) {
			ECS::Entity::Id meshEntityId = world->CreateEntity();
			meshsEntitiesIds.push_back(meshEntityId);
			world->CreateComponent<Mesh2>(meshEntityId);
			world->CreateComponent<Position>(meshEntityId, position->GetX(), position->GetY(), position->GetZ());
			world->CreateComponent<Rotation>(meshEntityId, rotation->GetRotationVector(), rotation->GetAngle());
			if (mesh.textureName_ != "") {
				world->CreateComponent<TextureInfo>(meshEntityId, mesh.textureName_);
			}
			
			OS::Assert(mesh.vertices_.GetVerticesNumber() != 0);
			world->CreateComponent<Vertices3D>(meshEntityId, mesh.vertices_);

			OS::Assert(mesh.indices_.GetIndicesNumber() != 0);
			world->CreateComponent<Indices>(meshEntityId, mesh.indices_);

			if (mesh.colors_.GetSize() != 0) {
				world->CreateComponent<Colors>(meshEntityId, mesh.colors_);
			}

			if (mesh.normals_.GetSize() != 0) {
				world->CreateComponent<Normals>(meshEntityId, mesh.normals_);
			}

			if (mesh.uvs_.GetSize() != 0) {
				world->CreateComponent<UVs>(meshEntityId, mesh.uvs_);
			}

		}

		world->CreateComponent<ChildEntities>(modelEntityid, meshsEntitiesIds);
		world->CreateComponent<ModelEntity>(entityId, modelEntityid);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromObjMtl::GetFilter() const noexcept {
		return { 
			ECS::Entity::Filter{}
			.Include<ObjEntity>()
			.Include<MtlEntity>()
			.Exclude<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateModelEntityFromObjMtl::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromObjMtl>().GetId();
	}
}