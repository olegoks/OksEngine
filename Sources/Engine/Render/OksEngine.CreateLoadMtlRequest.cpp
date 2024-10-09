
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/OksEngine.CreateLoadMtlRequest.hpp>

#include <Render/OksEngine.GeometryDescriptionFileEntity.hpp>
#include <Render/OksEngine.LoadMtlRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>

#include <Common/OksEngine.Name.hpp>
#include <Common/OksEngine.Text.hpp>
namespace OksEngine {


	void CreateLoadMtlRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {


		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		YAML::Node yaml = YAML::Load(geomFileText->text_);
		OS::Assert(!yaml.IsNull());
		if (yaml["Mtl"]) {
			const std::string mtlName = yaml["Mtl"].as<std::string>();
			const ECS::Entity::Id loadMtlResourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(loadMtlResourceEntityId);
			world->CreateComponent<Name>(loadMtlResourceEntityId, mtlName);

			world->CreateComponent<LoadMtlRequest>(entityId, loadMtlResourceEntityId);

		}
	}

	[[nodiscard]]
	std::string CreateLoadMtlRequest::GetName() const noexcept {
		return "CreateLoadMtlRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadMtlRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<GeometryDescriptionFileEntity>()
			.Exclude<LoadMtlRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadMtlRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadMtlRequest>().GetId();
	}

}