#pragma once
#include <Render\Texture\auto_OksEngine.Texture.hpp>

namespace OksEngine
{
	namespace Render
	{
		namespace NormalMap
		{
			void CreateTextureResource::Update(
				ECS2::Entity::Id entity0id,
				RenderDriver* renderDriver0,
				
				ECS2::Entity::Id entity1id,
				const TextureInfo* textureInfo1, 
				const TextureData* textureData1) {

				const ECS2::Entity::Id driverEntityId = entity0id;
				const ECS2::Entity::Id textureEntityId = entity1id;

				auto driver = renderDriver0->driver_;

				RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
					.name_ = textureInfo1->GetName(),
					.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
					.data_ = std::vector<Common::Byte>{
						(Common::Byte*)textureData1->pixels_.data(),
						(Common::Byte*)(textureData1->pixels_.data() + textureData1->pixels_.size()) },
					.size_ = { textureData1->width_, textureData1->height_ },
					.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
					.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
					.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader },
					.usages_ = {
						RAL::Driver::Texture::Usage::TransferDestination,	// To copy texture data to GPU.
						RAL::Driver::Texture::Usage::TransferSource,		// To generate mipmaps.
						RAL::Driver::Texture::Usage::Sampled },				// Sampler for shader.
		#undef max
					.mipLevels_ = static_cast<Common::UInt32>(std::floor(std::log2(std::max(textureData1->width_, textureData1->height_)))) + 1,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				};
				RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);

				ASSERT_FMSG(!textureId.IsInvalid(), "");


				RAL::Driver::Resource::Binding textureBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
					.binding_ = 0,
					.textureId_ = textureId
				};

				const RAL::Driver::Resource::Id textureRSId = driver->CreateResource(textureBinding);

				CreateComponent<Render::NormalMap::TextureResource>(textureEntityId, textureId, textureRSId);


			};

		}

	} // namespace Render

} // namespace OksEngine