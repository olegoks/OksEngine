
#include <Render/Texture/auto_OksEngine.CreateLoadTextureRequest.hpp>

namespace OksEngine {


	void CreateLoadTextureRequest::Update(TextureInfo* textureInfo) {

		//auto* textureInfo = world->GetComponent<TextureInfo>(entityId);

		//const ECS::Entity::Id loadTextureResourceEntityId = world->CreateEntity();

		//world->CreateComponent<LoadResourceRequest>(loadTextureResourceEntityId);
		//world->CreateComponent<Name>(loadTextureResourceEntityId, textureInfo->name_);
	
		//world->CreateComponent<LoadTextureRequest>(entityId, loadTextureResourceEntityId);

	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadTextureRequest::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		//.Include<Mesh2>()
	//		.Include<TextureInfo>()
	//		.Exclude<Texture>()
	//		.Exclude<LoadTextureRequest>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}


}