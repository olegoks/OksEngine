
#include <Render/Texture/auto_OksEngine.CreateDriverTexture.hpp>

//#include <Render/Texture/OksEngine.Texture.hpp>
//#include <Render/OksEngine.Render.Subsystem.hpp>
//#include <Render/Texture/OksEngine.TextureInfo.hpp>
//#include <Render/Texture/OksEngine.DriverTexture.hpp>
#include <RAL.Driver.hpp>

namespace OksEngine {


	void CreateDriverTexture::Update(
		ECS2::Entity::Id entity1Id,
		const TextureInfo* textureInfo,
		const Texture* texture,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		//auto* textureInfo = world->GetComponent<TextureInfo>(entityId);
		//auto* texture = world->GetComponent<Texture>(entityId);

		auto driver = renderDriver->driver_;
		std::vector<RAL::Color4b> texturePixels{ texture->pixels_.GetData(), texture->pixels_.GetData() + texture->pixels_.GetSize() };
		RAL::Texture::CreateInfo textureCreateInfo{
			.name_ = "",
			.pixels_ = texturePixels,
			.size_ = { texture->width_, texture->height_ }
		};
		RAL::Texture::Id textureId = driver->CreateDiffuseMap(textureCreateInfo);

		CreateComponent<DriverTexture>(entity1Id, textureId);
	}

	//[[nodiscard]]
	//std::string CreateDriverTexture::GetName() const noexcept {
	//	return "CreateDriverTexture";
	//}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverTexture::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<TextureInfo>()
	//		.Include<Texture>()
	//		.Exclude<DriverTexture>(), ECS::Entity::Filter{}.ExcludeAll() };

	//	return filter;
	//}

	//Common::TypeId CreateDriverTexture::GetTypeId() const noexcept {
	//	return Common::TypeInfo<CreateDriverTexture>().GetId();
	//}

}