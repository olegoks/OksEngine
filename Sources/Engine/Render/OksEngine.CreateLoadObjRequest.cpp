
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/OksEngine.CreateLoadObjRequest.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadObjRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.GeometryDescriptionFileEntity.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Common/OksEngine.Text.hpp>

namespace OksEngine {


	void CreateLoadObjRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		YAML::Node yaml = YAML::Load(geomFileText->text_);

		if (yaml["Obj"]) {
			const std::string objName = yaml["Obj"].as<std::string>();
			const ECS::Entity::Id loadObjResourceEntityId = world->CreateEntity();
			{
				world->CreateComponent<LoadResourceRequest>(loadObjResourceEntityId);
				world->CreateComponent<Name>(loadObjResourceEntityId, objName);
			}
			world->CreateComponent<LoadObjRequest>(entityId, loadObjResourceEntityId);

		}
	}

	[[nodiscard]]
	std::string CreateLoadObjRequest::GetName() const noexcept {
		return "CreateLoadObjRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadObjRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<GeometryDescriptionFileEntity>()
			.Exclude<LoadObjRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadObjRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadObjRequest>().GetId();
	}

}