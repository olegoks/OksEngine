#pragma once
#include <Render\Texture\auto_OksEngine.Render.Texture.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Texture
		{
			void CreateDriverTexture::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				ECS2::Entity::Id entity1id, const OksEngine::Render::Texture::Tag* texture__Tag1,
				const OksEngine::Render::Texture::Data* texture__Data1,
				const OksEngine::Render::Texture::Info* texture__Info1) {


				RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
					.name_ = "",
					.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
					.data_ = std::vector<Common::Byte>{
						(const Common::Byte*)texture__Data1->pixels_.data(),
						(const Common::Byte*)texture__Data1->pixels_.data() + texture__Data1->pixels_.size() * 4},
					.size_ = {
						texture__Data1->width_,
						texture__Data1->height_ },
					.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
					.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
					.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader, RAL::Driver::Pipeline::Stage::VertexShader},
					.usages_ = { RAL::Driver::Texture::Usage::Sampled, RAL::Driver::Texture::Usage::TransferDestination },
					.mipLevels_ = 1,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				};
				RAL::Driver::Texture::Id textureId = renderDriver0->driver_->CreateTexture(textureCreateInfo);

				CreateComponent<Texture::DriverId>(entity1id, textureId);

			};

		}

	} // namespace Render

} // namespace OksEngine