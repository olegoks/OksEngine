#include <Render/Texture/auto_OksEngine.CreateTexture.hpp>


namespace OksEngine {


	void CreateTexture::Update(LoadTextureRequest* loadTextureRequest) {

		//auto* loadTextureRequest = world->GetComponent<LoadTextureRequest>(entityId);
		//if (world->IsComponentExist<ResourceEntity>(loadTextureRequest->resourceLoadRequestEntityId_)) {
		//	auto* resourceEntity = world->GetComponent<ResourceEntity>(loadTextureRequest->resourceLoadRequestEntityId_);
		//	auto* binaryData = world->GetComponent<BinaryData>(resourceEntity->id_);

		//	Geom::Texture texture = Geom::CreateTexture(binaryData->data_.data(), binaryData->data_.size());
		//	world->CreateComponent<Texture>(entityId, texture.GetWidth(), texture.GetHeight(), texture.GetPixelsArray());
		//}
		//
	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateTexture::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = 
	//	{ ECS::Entity::Filter{}
	//	.Include<LoadTextureRequest>()
	//	.Exclude<Texture>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}


}