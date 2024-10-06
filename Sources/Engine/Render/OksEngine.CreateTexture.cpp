#include <Render/OksEngine.CreateTexture.hpp>

#include <Render/OksEngine.GeometryFile.hpp>
#include <Render/OksEngine.LoadTextureRequest.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.Texture.hpp>

namespace OksEngine {


	void CreateTexture::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadTextureRequest = world->GetComponent<LoadTextureRequest>(entityId);
		if (world->IsComponentExist<Resource>(loadTextureRequest->resourceTaskEntityId_)) {
			auto* textureResource = world->GetComponent<Resource>(loadTextureRequest->resourceTaskEntityId_);

			Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
			world->CreateComponent<Texture>(entityId, texture.GetWidth(), texture.GetHeight(), texture.GetPixelsArray());
		}
		
	}

	[[nodiscard]]
	std::string CreateTexture::GetName() const noexcept {
		return "CreateTexture";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateTexture::GetFilter() const noexcept {

		return { ECS::Entity::Filter{}
			.Include<LoadTextureRequest>()
			.Exclude<Texture>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateTexture::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateTexture>().GetId();
	}

}