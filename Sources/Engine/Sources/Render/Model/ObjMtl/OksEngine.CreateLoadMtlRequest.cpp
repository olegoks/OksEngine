
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/Model/ObjMtl/auto_OksEngine.CreateLoadMtlRequest.hpp>

namespace OksEngine {


	void CreateLoadMtlRequest::Update(
		ECS2::Entity::Id entityId,
		const GDF* GDF) {


		//auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		//auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		//YAML::Node yaml = YAML::Load(geomFileText->value_);
		//OS::Assert(!yaml.IsNull());
		//if (yaml["Mtl"]) {
		//	const std::string mtlName = yaml["Mtl"].as<std::string>();
		//	const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
		//	world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId);
		//	world->CreateComponent<Name>(loadMtlResourceEntityId, mtlName);

		//	world->CreateComponent<LoadMtlRequest>(entityId, loadMtlResourceEntityId);

		//}
	}

	//[[nodiscard]]
	//std::string CreateLoadMtlRequest::GetName() const noexcept {
	//	return "CreateLoadMtlRequest";
	//}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadMtlRequest::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<GeometryDescriptionFileEntity>()
	//		.Exclude<LoadMtlRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId CreateLoadMtlRequest::GetTypeId() const noexcept {
	//	return Common::TypeInfo<CreateLoadMtlRequest>().GetId();
	//}

}