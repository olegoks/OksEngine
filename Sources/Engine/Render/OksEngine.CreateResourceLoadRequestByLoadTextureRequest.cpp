#include <Render/OksEngine.CreateResourceLoadRequestByLoadTextureRequest.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadTextureRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.Texture.hpp>

namespace OksEngine {


	void CreateResourceLoadRequestByLoadTextureRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* loadTextureRequest = world->GetComponent<LoadTextureRequest>(entityId);



	}

	[[nodiscard]]
	std::string CreateResourceLoadRequestByLoadTextureRequest::GetName() const noexcept {
		return "CreateResourceLoadRequestByLoadTextureRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateResourceLoadRequestByLoadTextureRequest::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<LoadTextureRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateResourceLoadRequestByLoadTextureRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateResourceLoadRequestByLoadTextureRequest>().GetId();
	}

}