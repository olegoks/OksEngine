#include <Render/Texture/OksEngine.CreateResourceLoadRequestByLoadTextureRequest.hpp>

#include <Render/GeometryDescriptionFile/OksEngine.GeometryDescriptionFile.hpp>
#include <Render/Texture/OksEngine.LoadTextureRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/Texture/OksEngine.Texture.hpp>

namespace OksEngine {


	void CreateResourceLoadRequestByLoadTextureRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* loadTextureRequest = world->GetComponent<LoadTextureRequest>(entityId);



	}

	[[nodiscard]]
	std::string CreateResourceLoadRequestByLoadTextureRequest::GetName() const noexcept {
		return "CreateResourceLoadRequestByLoadTextureRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateResourceLoadRequestByLoadTextureRequest::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<LoadTextureRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateResourceLoadRequestByLoadTextureRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateResourceLoadRequestByLoadTextureRequest>().GetId();
	}

}