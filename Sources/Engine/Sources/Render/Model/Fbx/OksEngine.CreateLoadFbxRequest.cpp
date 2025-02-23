
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/Model/Fbx/auto_OksEngine.CreateLoadFbxRequest.hpp>

namespace OksEngine {


	void CreateLoadFbxRequest::Update(
		ECS2::Entity::Id entityId,
		const GeometryDescriptionFile* geometryDescriptionFile) {

		//auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		//auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		//YAML::Node yaml = YAML::Load(geomFileText->value_);

		//if (yaml["Fbx"]) {
		//	const std::string fbxName = yaml["Fbx"].as<std::string>();
		//	const ECS::Entity::Id loadFbxResourceEntityId = world->CreateEntity();
		//	{
		//		world->CreateComponent<LoadResourceRequest>(loadFbxResourceEntityId);
		//		world->CreateComponent<Name>(loadFbxResourceEntityId, fbxName);
		//	}
		//	world->CreateComponent<LoadFbxRequest>(entityId, loadFbxResourceEntityId);

		//}
	}


}