
#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

#include <Render/Model/Fbx/OksEngine.CreateLoadFbxRequest.hpp>

#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFile.hpp>
#include <Render/Model/Fbx/OksEngine.LoadFbxRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFileEntity.hpp>
#include <Common/auto_OksEngine.Name.hpp>
#include <Common/auto_OksEngine.Text.hpp>

namespace OksEngine {


	void CreateLoadFbxRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);
		auto* geomFileText = world->GetComponent<Text>(geomFileEntity->id_);

		YAML::Node yaml = YAML::Load(geomFileText->value_);

		if (yaml["Fbx"]) {
			const std::string fbxName = yaml["Fbx"].as<std::string>();
			const ECS::Entity::Id loadFbxResourceEntityId = world->CreateEntity();
			{
				world->CreateComponent<LoadResourceRequest>(loadFbxResourceEntityId);
				world->CreateComponent<Name>(loadFbxResourceEntityId, fbxName);
			}
			world->CreateComponent<LoadFbxRequest>(entityId, loadFbxResourceEntityId);

		}
	}

	[[nodiscard]]
	std::string CreateLoadFbxRequest::GetName() const noexcept {
		return "CreateLoadFbxRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadFbxRequest::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<GeometryDescriptionFileEntity>()
			.Exclude<LoadFbxRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateLoadFbxRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadFbxRequest>().GetId();
	}

}