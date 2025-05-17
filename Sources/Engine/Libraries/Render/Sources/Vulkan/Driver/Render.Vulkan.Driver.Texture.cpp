
#include <Render.Vulkan.Driver.Texture.hpp>

#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.CommandBuffer.hpp>

namespace Render::Vulkan {


	Texture::Texture(const CreateInfo& createInfo) : RAL::Texture{ createInfo.ralCreateInfo_ }{
			OS::Assert(createInfo.format_ == VK_FORMAT_R8G8B8A8_UNORM);

			auto textureStagingBuffer = std::make_shared<StagingBuffer>(createInfo.PD_, createInfo.LD_, GetSize().x * GetSize().y * sizeof(RAL::Color4b));
			textureStagingBuffer->Allocate();
			textureStagingBuffer->Fill(0, GetPixels().data(), GetSize().x* GetSize().y * sizeof(RAL::Color4b));

			AllocatedTextureImage::CreateInfo textureImageCreateInfo;
			{
				textureImageCreateInfo.size_ = GetSize();
				textureImageCreateInfo.format_ = createInfo.format_;
				textureImageCreateInfo.LD_ = createInfo.LD_;
				textureImageCreateInfo.physicalDevice_ = createInfo.PD_;
				textureImageCreateInfo.mipLevels_ = createInfo.mipLevels_;
			}
			auto image = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);

			image->ChangeLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, createInfo.commandPool_);
			Image::DataCopy(textureStagingBuffer, image, createInfo.LD_, createInfo.commandPool_);

			const bool mipMapGenerationEnabled = createInfo.PD_->SupportsMipMaps(image->GetFormat());

			if(mipMapGenerationEnabled)	{

				CommandBuffer::CreateInfo commandBufferCreateInfo;
				{
					commandBufferCreateInfo.LD_ = createInfo.LD_;
					commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
					commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				}
				auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

				int32_t mipWidth = createInfo.ralCreateInfo_.size_.x;
				int32_t mipHeight = createInfo.ralCreateInfo_.size_.y;

				commandBuffer->Begin();

				for (uint32_t i = 1; i < createInfo.mipLevels_; i++) {

					commandBuffer->ImageMemoryBarrier(*image,
						i - 1, 1, 
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
						);

					VkOffset3D srcOfffsets[2]{ { 0, 0, 0 }, { mipWidth, mipHeight, 1 } };
					VkOffset3D dstOfffsets[2]{ { 0, 0, 0 }, mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };

					commandBuffer->ImageBlit(
						image,
						srcOfffsets,
						i - 1,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						image,
						dstOfffsets,
						i,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						);

					commandBuffer->ImageMemoryBarrier(*image,
						i - 1, 1,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					);

					if (mipWidth > 1) mipWidth /= 2;
					if (mipHeight > 1) mipHeight /= 2;
				}

				commandBuffer->ImageMemoryBarrier(*image,
					createInfo.mipLevels_ - 1, 1,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				);

				commandBuffer->End();
				commandBuffer->Submit(createInfo.LD_->GetGraphicsQueue());

				vkQueueWaitIdle(createInfo.LD_->GetGraphicsQueue());

			} else {
				image->ChangeLayout(
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					createInfo.commandPool_);
			}

			auto imageView = CreateImageViewByImage(createInfo.LD_, image, VK_IMAGE_ASPECT_COLOR_BIT, createInfo.mipLevels_);

			Sampler::CreateInfo samplerCreateInfo;
			{
				samplerCreateInfo.LD_ = createInfo.LD_;
				samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.maxAnisotropy_ = createInfo.PD_->GetProperties().limits.maxSamplerAnisotropy;
				samplerCreateInfo.mipLevels_ = (mipMapGenerationEnabled) ? (createInfo.mipLevels_) : (1);
			}
			auto sampler = std::make_shared<Sampler>(samplerCreateInfo);

			//auto DSL = std::make_shared<DescriptorSetLayout>(
			//	DescriptorSetLayout::CreateInfo{
			//		"DiffuseMap",
			//		createInfo.LD_,
			//		std::vector<VkDescriptorSetLayoutBinding>{{
			//			{
			//		0,
			//		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			//		1,
			//		VK_SHADER_STAGE_FRAGMENT_BIT,
			//		nullptr
			//		}
			//		}}
			//	});

			//DescriptorSet::CreateInfo DSCreateInfo;
			//{
			//	DSCreateInfo.DP_ = createInfo.DP_;
			//	DSCreateInfo.DSL_ = DSL;
			//	DSCreateInfo.LD_ = createInfo.LD_;
			//}

			//auto DS = std::make_shared<DescriptorSet>(DSCreateInfo);
			//DS->UpdateImageWriteConfiguration(
			//	imageView,
			//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			//	sampler,
			//	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			//	0);

			image_ = image;
			imageView_ = imageView;
			sampler_ = sampler;
	/*		DSL_ = DSL;
			DS_ = DS;*/

		}

}