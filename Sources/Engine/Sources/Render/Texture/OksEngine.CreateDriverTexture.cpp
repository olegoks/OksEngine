
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

		RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
			.name_ = textureInfo->GetName(),
			.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
			.data_ = std::vector<Common::Byte>{
				(Common::Byte*)texture->pixels_.data(),
				(Common::Byte*)(texture->pixels_.data() + texture->pixels_.size() /** RAL::Driver::Texture::GetElementSize(RAL::Driver::Texture::Format::RGBA_32_UNORM)*/) },
			.size_ = { texture->width_, texture->height_ },
			.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
			.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
			.targetStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader },
			.usages_ = { 
				RAL::Driver::Texture::Usage::TransferDestination,	// To copy texture data to GPU.
				RAL::Driver::Texture::Usage::TransferSource,		// To generate mipmaps.
				RAL::Driver::Texture::Usage::Sampled },				// Sampler for shader.
#undef max
			.mipLevels_ = static_cast<Common::UInt32>(std::floor(std::log2(std::max(texture->width_, texture->height_)))) + 1,
		};
		RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);


#pragma region Assert
		OS::AssertMessage(!textureId.IsInvalid(), "");
#pragma endregion


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