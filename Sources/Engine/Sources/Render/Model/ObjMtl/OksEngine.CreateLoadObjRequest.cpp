
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/Model/ObjMtl/auto_OksEngine.CreateLoadObjRequest.hpp>


namespace OksEngine {


	void CreateLoadObjRequest::Update(
		ECS2::Entity::Id entityId,
		const GeometryDescriptionFile* geometryDescriptionFile) {

		//auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		//auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		//YAML::Node yaml = YAML::Load(geomFileText->value_);

		//if (yaml["Obj"]) {
		//	const std::string objName = yaml["Obj"].as<std::string>();
		//	const ECS::Entity::Id loadObjResourceEntityId = world->CreateEntity();
		//	{
		//		world->CreateComponent<LoadResourceRequest>(loadObjResourceEntityId);
		//		world->CreateComponent<Name>(loadObjResourceEntityId, objName);
		//	}
		//	world->CreateComponent<LoadObjRequest>(entityId, loadObjResourceEntityId);

		//}
	}



	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadObjRequest::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<GeometryDescriptionFileEntity>()
	//		.Exclude<LoadObjRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}


}