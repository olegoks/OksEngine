
#include <Render/Model/OksEngine.CreateModel.hpp>

#include <Animation/OksEngine.Animation.Components.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

namespace OksEngine {

	void CreateModelEntityFromObjMtl::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

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
		OS::Assert(model2.meshes_.size() == 1);
		for (auto& mesh : model2.meshes_) {
			//ECS::Entity::Id meshEntityId = world->CreateEntity();
			//meshsEntitiesIds.push_back(meshEntityId);
			world->CreateComponent<Mesh2>(entityId);
			//world->CreateComponent<Position>(meshEntityId, position->GetX(), position->GetY(), position->GetZ());
			//world->CreateComponent<Rotation>(meshEntityId, rotation->GetRotationVector(), rotation->GetAngle());
			if (mesh.textureName_ != "") {
				world->CreateComponent<TextureInfo>(entityId, mesh.textureName_);
			}
			
			OS::Assert(mesh.vertices_.GetVerticesNumber() != 0);
			world->CreateComponent<Vertices3D>(entityId, mesh.vertices_);

			OS::Assert(mesh.indices_.GetIndicesNumber() != 0);
			world->CreateComponent<Indices>(entityId, mesh.indices_);

			if (mesh.colors_.GetSize() != 0) {
				world->CreateComponent<Colors>(entityId, mesh.colors_);
			}

			if (mesh.normals_.GetSize() != 0) {
				world->CreateComponent<Normals>(entityId, mesh.normals_);
			}

			if (mesh.uvs_.GetSize() != 0) {
				world->CreateComponent<UVs>(entityId, mesh.uvs_);
			}

		}

		//world->CreateComponent<ChildEntities>(modelEntityid, meshsEntitiesIds);
		//world->CreateComponent<ModelEntity>(entityId, modelEntityid);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromObjMtl::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<ObjEntity>()
			.Include<MtlEntity>()
			.Exclude<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateModelEntityFromObjMtl::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromObjMtl>().GetId();
	}


	void CreateModelEntityFromFbx::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position>(entityId);
		auto* rotation = world->GetComponent<Rotation>(entityId);
		auto* fbxEntity = world->GetComponent<FbxEntity>(entityId);
		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);

		ECS::Entity::Id modelEntityid = world->CreateEntity();
		std::vector<ECS::Entity::Id> meshsEntitiesIds;
		auto* fbxName = world->GetComponent<Name>(fbxEntity->id_);
		auto* fbxData = world->GetComponent<BinaryData>(fbxEntity->id_);

		Geom::Model2 model2 = Geom::ParseFbxModelBaked(
			fbxName->value_,
			std::string{ fbxData->data_.data(), fbxData->data_.size() } );

		if (!model2.animation_->states_.empty()) {
			std::vector<Animation::StateInfo> states;
			for (Geom::Model2::Animation::StateInfo& state : model2.animation_->states_) {
				states.push_back({ state.time_, state.position_});
			}
			world->CreateComponent<Animation>(entityId, model2.animation_->ticksNumber_, model2.animation_->ticksPerSecond_, states);
		}




		for (auto& mesh : model2.meshes_) {
			//ECS::Entity::Id meshEntityId = world->CreateEntity();
			//meshsEntitiesIds.push_back(meshEntityId);
			world->CreateComponent<Mesh2>(entityId);
			//world->CreateComponent<Position>(entityId, position->GetX(), position->GetY(), position->GetZ());
			//world->CreateComponent<Rotation>(entityId, rotation->GetRotationVector(), rotation->GetAngle());
			if (mesh.textureName_ != "") {
				world->CreateComponent<TextureInfo>(entityId, mesh.textureName_);
			}

			OS::Assert(mesh.vertices_.GetVerticesNumber() != 0);
			world->CreateComponent<Vertices3D>(entityId, mesh.vertices_);

			OS::Assert(mesh.indices_.GetIndicesNumber() != 0);
			world->CreateComponent<Indices>(entityId, mesh.indices_);

			if (mesh.colors_.GetSize() != 0) {
				world->CreateComponent<Colors>(entityId, mesh.colors_);
			}

			if (mesh.normals_.GetSize() != 0) {
				world->CreateComponent<Normals>(entityId, mesh.normals_);
			}

			if (mesh.uvs_.GetSize() != 0) {
				world->CreateComponent<UVs>(entityId, mesh.uvs_);
			}

		}

		//world->CreateComponent<ChildEntities>(modelEntityid, meshsEntitiesIds);
		//world->CreateComponent<ModelEntity>(entityId, modelEntityid);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromFbx::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<FbxEntity>()
			.Exclude<Vertices3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateModelEntityFromFbx::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromFbx>().GetId();
	}


}