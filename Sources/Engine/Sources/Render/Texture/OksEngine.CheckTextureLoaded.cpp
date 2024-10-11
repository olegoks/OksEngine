
#include <Render/Texture/OksEngine.CheckTextureLoaded.hpp>

#include <Render/Texture/OksEngine.LoadTextureRequest.hpp>
#include <Render/Texture/OksEngine.TextureInfo.hpp>
#include <Render/Texture/OksEngine.Texture.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	void CheckTextureLoaded::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* textureInfo = world->GetComponent<TextureInfo>(entityId);
		auto* loadTextureRequest = world->GetComponent<LoadTextureRequest>(entityId);


		//loadTextureRequest->resourceLoadRequestEntityId_;
		//auto* loadResourceRequest = world->GetComponent<LoadResourceRequest>(secondEntityId);
		//auto* textureResource = world->GetComponent<Resource>(secondEntityId);

		//if (loadResourceRequest->GetName() == loadTextureRequest->GetName()) {
		//	Geom::Texture texture = Geom::CreateTexture(textureResource->resourceData_.c_str(), textureResource->resourceData_.size());
		//	world->CreateComponent<Texture>(entityId, texture.GetWidth(), texture.GetHeight(), texture.GetPixelsArray());
		//}

	}

	std::string CheckTextureLoaded::GetName() const noexcept
	{
		return "CheckTextureLoaded";
	}

	inline std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CheckTextureLoaded::GetFilter() const noexcept {
		return { 
			ECS::Entity::Filter{}
			.Include<TextureInfo>()
			.Include<LoadTextureRequest>()
			.Exclude<Texture>(), 
			ECS::Entity::Filter{}
			.Include<LoadResourceRequest>()
			.Include<Resource>() };
	}

	inline Common::TypeId CheckTextureLoaded::GetTypeId() const noexcept {
		return Common::TypeInfo<CheckTextureLoaded>().GetId();
	}

	CheckTextureLoaded::CheckTextureLoaded(Context& context) noexcept :
		ECSSystem{ context } { }

}