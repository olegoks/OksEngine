
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
			auto textureStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.LD_, GetSize().x * GetSize().y * sizeof(RAL::Color4b));
			textureStagingBuffer->Fill(GetPixels().data());

			AllocatedTextureImage::CreateInfo textureImageCreateInfo;
			{
				textureImageCreateInfo.size_ = GetSize();
				textureImageCreateInfo.format_ = createInfo.format_;
				textureImageCreateInfo.LD_ = createInfo.LD_;
				textureImageCreateInfo.physicalDevice_ = createInfo.physicalDevice_;
				textureImageCreateInfo.mipLevels_ = createInfo.mipLevels_;
			}
			auto image = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);

			//Generating mip maps.
			
			const VkFormatProperties formatProperties = createInfo.physicalDevice_->GetFormatProperties(image->GetFormat());
			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
				OS::AssertFailMessage("Mip map levels generation is not supported by device.");
			}
			{
				CommandBuffer::CreateInfo commandBufferCreateInfo;
				{
					commandBufferCreateInfo.LD_ = createInfo.LD_;
					commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
				}
				auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
				commandBuffer->Begin();

				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.image = *image;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;
				barrier.subresourceRange.levelCount = 1;

				int32_t mipWidth = createInfo.ralCreateInfo_.size_.x;
				int32_t mipHeight = createInfo.ralCreateInfo_.size_.y;

				for (uint32_t i = 1; i < createInfo.mipLevels_; i++) {
					barrier.subresourceRange.baseMipLevel = i - 1;
					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

					vkCmdPipelineBarrier(*commandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier);

					VkImageBlit blit{};
					blit.srcOffsets[0] = { 0, 0, 0 };
					blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
					blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.srcSubresource.mipLevel = i - 1;
					blit.srcSubresource.baseArrayLayer = 0;
					blit.srcSubresource.layerCount = 1;
					blit.dstOffsets[0] = { 0, 0, 0 };
					blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
					blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					blit.dstSubresource.mipLevel = i;
					blit.dstSubresource.baseArrayLayer = 0;
					blit.dstSubresource.layerCount = 1;

					vkCmdBlitImage(*commandBuffer,
						*image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					vkCmdPipelineBarrier(*commandBuffer,
						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
						0, nullptr,
						0, nullptr,
						1, &barrier);

					if (mipWidth > 1) mipWidth /= 2;
					if (mipHeight > 1) mipHeight /= 2;
				}

				barrier.subresourceRange.baseMipLevel = createInfo.mipLevels_ - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(*commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				commandBuffer->End();


			}

			image->ChangeLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, createInfo.commandPool_);
			Image::DataCopy(textureStagingBuffer, image, createInfo.LD_, createInfo.commandPool_);
			image->ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, createInfo.commandPool_);
			auto imageView = CreateImageViewByImage(createInfo.LD_, image, VK_IMAGE_ASPECT_COLOR_BIT, createInfo.mipLevels_);

			Sampler::CreateInfo samplerCreateInfo;
			{
				samplerCreateInfo.LD_ = createInfo.LD_;
				samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.maxAnisotropy_ = createInfo.physicalDevice_->GetProperties().limits.maxSamplerAnisotropy;
				samplerCreateInfo.mipLevels_ = createInfo.mipLevels_;
			}
			auto sampler = std::make_shared<Sampler>(samplerCreateInfo);

			auto DSL = std::make_shared<DescriptorSetLayout>(
				DescriptorSetLayout::CreateInfo{
					"DiffuseMap",
					createInfo.LD_,
					std::vector<VkDescriptorSetLayoutBinding>{{
						{
					0,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					1,
					VK_SHADER_STAGE_FRAGMENT_BIT,
					nullptr
					}
					}}
				});

			DescriptorSet::CreateInfo DSCreateInfo;
			{
				DSCreateInfo.DP_ = createInfo.DP_;
				DSCreateInfo.DSL_ = DSL;
				DSCreateInfo.LD_ = createInfo.LD_;
			}

			auto DS = std::make_shared<DescriptorSet>(DSCreateInfo);
			DS->UpdateImageWriteConfiguration(
				imageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				sampler,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				0);

			image_ = image;
			imageView_ = imageView;
			sampler_ = sampler;
			DSL_ = DSL;
			DS_ = DS;

		}

}