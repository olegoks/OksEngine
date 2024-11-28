
#include <Render/Texture/OksEngine.CreateDriverTexture.hpp>

#include <Render/Texture/OksEngine.Texture.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Texture/OksEngine.TextureInfo.hpp>
#include <Render/Texture/OksEngine.DriverTexture.hpp>
#include <RAL.Driver.hpp>

namespace OksEngine {


	void CreateDriverTexture::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* textureInfo = world->GetComponent<TextureInfo>(entityId);
		auto* texture = world->GetComponent<Texture>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		std::vector<RAL::Color4b> texturePixels{ texture->pixels_.GetData(), texture->pixels_.GetData() + texture->pixels_.GetSize() };
		RAL::Texture::CreateInfo textureCreateInfo{
			.name_ = "",
			.pixels_ = texturePixels,
			.size_ = { texture->width_, texture->height_ }
		};
		RAL::Texture::Id textureId = driver->CreateDiffuseMap(textureCreateInfo);

		world->CreateComponent<DriverTexture>(entityId, textureId);
	}

	[[nodiscard]]
	std::string CreateDriverTexture::GetName() const noexcept {
		return "CreateDriverTexture";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverTexture::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<TextureInfo>()
			.Include<Texture>()
			.Exclude<DriverTexture>(), ECS::Entity::Filter{}.ExcludeAll() };

		return filter;
	}

	Common::TypeId CreateDriverTexture::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverTexture>().GetId();
	}

}