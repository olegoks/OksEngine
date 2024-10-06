
#include <Render/OksEngine.CreateLoadTextureRequest.hpp>

#include <Render/OksEngine.LoadObjRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.Mesh.hpp>
#include <Render/OksEngine.TextureInfo.hpp>
#include <Render/OksEngine.LoadTextureRequest.hpp>

namespace OksEngine {


	void CreateLoadTextureRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* textureInfo = world->GetComponent<TextureInfo>(entityId);

		const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();
		world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId, textureInfo->name_);
		world->CreateComponent<LoadTextureRequest>(entityId, loadTextureResourceEntityId);
		

	}

	[[nodiscard]]
	std::string CreateLoadTextureRequest::GetName() const noexcept {
		return "CreateLoadTextureRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadTextureRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<Mesh2>()
			.Include<TextureInfo>()
			.Exclude<LoadTextureRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateLoadTextureRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadTextureRequest>().GetId();
	}

}