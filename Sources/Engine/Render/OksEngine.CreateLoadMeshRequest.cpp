

#include <Render/OksEngine.CreateLoadMeshRequest.hpp>

#include <Render/OksEngine.MapMeshTransform.hpp>
#include <Render/OksEngine.LoadMeshRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.GeometryFile.hpp>


namespace OksEngine {

	void CreateLoadMeshRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
	//	auto* geomFile = world->GetComponent<GeometryDescriptionFile>(entityId);
	//	std::map<std::string, ECS::Entity::Id> resourceEntityId_;
	//	LoadMeshRequest::Type meshType = LoadMeshRequest::Type::Undefined;
	//	if (geomFile->mesh_["Obj"]) {

	//		const std::string objName = geomFile->mesh_["Obj"].as<std::string>();
	//		const ECS::Entity::Id loadObjResourceEntityId = world->CreateEntity();
	//		world->CreateComponent<LoadResourceRequest>(loadObjResourceEntityId, objName);
	//		resourceEntityId_[objName] = loadObjResourceEntityId;
	//		meshType = LoadMeshRequest::Type::OBJ;

	//		if (geomFile->mesh_["Mtl"]) {
	//			const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
	//			const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
	//			world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId, mtlName);
	//			resourceEntityId_[mtlName] = loadMtlResourceEntityId;

	//			meshType = LoadMeshRequest::Type::OBJ_MTL;

	//			if (geomFile->mesh_["Textures"] && geomFile->mesh_["Textures"].size() != 0) {
	//				for (Common::Index i = 0; i < geomFile->mesh_["Textures"].size(); i++) {
	//					const std::string textureName = geomFile->mesh_["Textures"][i].as<std::string>();
	//					const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();
	//					world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId, textureName);
	//					resourceEntityId_[textureName] = loadTextureResourceEntityId;
	//				}
	//				meshType = LoadMeshRequest::Type::OBJ_MTL_TEXTURES;
	//			}
	//		}
	//	}
	///*	if (geomFile->mesh_["Fbx"]) {

	//		const std::string fbxName = geomFile->mesh_["Fbx"].as<std::string>();
	//		const ECS::Entity::Id loadFbxResourceEntityId = world->CreateEntity();
	//		world->CreateComponent<LoadResourceRequest>(loadFbxResourceEntityId, fbxName);
	//		resourceEntityId_[fbxName] = loadFbxResourceEntityId;
	//		meshType = LoadMeshRequest::Type::FBX_TEXTURES;

	//		if (geomFile->mesh_["Mtl"]) {
	//			const std::string mtlName = geomFile->mesh_["Mtl"].as<std::string>();
	//			const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
	//			world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId, mtlName);
	//			resourceEntityId_[mtlName] = loadMtlResourceEntityId;

	//			meshType = LoadMeshRequest::Type::OBJ_MTL;

	//			if (geomFile->mesh_["Textures"] && geomFile->mesh_["Textures"].size() != 0) {
	//				for (Common::Index i = 0; i < geomFile->mesh_["Textures"].size(); i++) {
	//					const std::string textureName = geomFile->mesh_["Textures"][i].as<std::string>();
	//					const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();
	//					world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId, textureName);
	//					resourceEntityId_[textureName] = loadTextureResourceEntityId;
	//				}
	//				meshType = LoadMeshRequest::Type::OBJ_MTL_TEXTURES;
	//			}
	//		}
	//	}*/
	//	OS::AssertMessage(meshType != LoadMeshRequest::Type::Undefined, { "Maybe %s contains incorrect data!", geomFile->geomName_.c_str() });
	//	const std::string name = geomFile->mesh_["Name"].as<std::string>();
	//	world->CreateComponent<LoadMeshRequest>(entityId, name, resourceEntityId_, meshType);

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadMeshRequest::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<GeometryDescriptionFile>()
			.Exclude<LoadMeshRequest>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadMeshRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadMeshRequest>().GetId();
	}

}