
#include <Render.Vulkan.Driver.Texture.hpp>

#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
#include <Render.Vulkan.Driver.CommandBuffer.hpp>
#include <Render.Vulkan.Driver.hpp>

namespace Render::Vulkan {

	static VkImageAspectFlags GetAspectByFormat(VkFormat format) {
		switch (format) {
			// Цветовые форматы
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_B8G8R8A8_UINT:
		case VK_FORMAT_B8G8R8A8_SRGB:
			return VK_IMAGE_ASPECT_COLOR_BIT;

			// Форматы глубины
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM:
			return VK_IMAGE_ASPECT_DEPTH_BIT;

			// Комбинированные форматы глубины/трафарета
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

			// Трафарет
		case VK_FORMAT_S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT;

		default:
			ASSERT_FAIL_MSG("Unknown format to get aspect.");
			return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
		}

	}

	Texture::Texture(const CreateInfo1& createInfo) {

		const Common::Size pixelsNumber = createInfo.size_.x * createInfo.size_.y;
		const Common::Size pixelSize = Driver::GetElementSize(createInfo.format_);


		const bool mipMapGenerationEnabled = createInfo.PD_->SupportsMipMaps(createInfo.format_);

		AllocatedTextureImage::CreateInfo textureImageCreateInfo;
		{
			textureImageCreateInfo.size_ = createInfo.size_;
			textureImageCreateInfo.format_ = createInfo.format_;
			textureImageCreateInfo.usage_ = createInfo.usages_;
			textureImageCreateInfo.LD_ = createInfo.LD_;
			textureImageCreateInfo.physicalDevice_ = createInfo.PD_;
			textureImageCreateInfo.mipLevels_ = (mipMapGenerationEnabled) ? (createInfo.mipLevels_) : (1);
			textureImageCreateInfo.samplesCount_ = createInfo.samplesCount_;
		}
		auto image = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);

		VkImageLayout currentLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		VkAccessFlags currentAccess = VkAccessFlagBits::VK_ACCESS_NONE;
		VkPipelineStageFlags currentPipelineStage = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		//Copy data if need.
		if (!createInfo.data_.empty()) {

			auto textureStagingBuffer = std::make_shared<StagingBuffer>(
				createInfo.PD_,
				createInfo.LD_,
				pixelsNumber * pixelSize);

			textureStagingBuffer->Allocate();

			ASSERT_FMSG((createInfo.data_.size() % pixelSize) == 0, "");
			ASSERT_FMSG((createInfo.data_.size() / pixelSize) == pixelsNumber, "");

			textureStagingBuffer->Write(0, createInfo.data_.data(), pixelsNumber * pixelSize);

			//Change image layout(all mip map levels) to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.name_ = "Change image layout command buffer";
				commandBufferCreateInfo.LD_ = createInfo.LD_;
				commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
				commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
			commandBuffer->Begin();
			
			commandBuffer->ImageMemoryBarrier(
				image,
				0,
				createInfo.mipLevels_,
				GetAspectByFormat(createInfo.format_),
				currentLayout,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
			);
			commandBuffer->End();
			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
			}
			vkQueueSubmit(createInfo.LD_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(createInfo.LD_->GetGraphicsQueue());

			currentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			currentAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
			currentPipelineStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			//Copy data from staging buffer.
			Image::DataCopy(textureStagingBuffer, image, createInfo.LD_, createInfo.commandPool_);

		}

		if (mipMapGenerationEnabled && createInfo.mipLevels_ > 1) {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.name_ = "Generating image mip maps command buffer";
				commandBufferCreateInfo.LD_ = createInfo.LD_;
				commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
				commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

			int32_t mipWidth = createInfo.size_.x;
			int32_t mipHeight = createInfo.size_.y;

			commandBuffer->Begin();

			for (uint32_t i = 1; i < createInfo.mipLevels_; i++) {

				// Previous mip map level as source.
				commandBuffer->ImageMemoryBarrier(image,
					i - 1, 1,
					GetAspectByFormat(createInfo.format_),
					currentLayout,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					currentAccess, VK_ACCESS_TRANSFER_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT
				);

				if (currentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
					// Current mip map level as destination.
					commandBuffer->ImageMemoryBarrier(image,
						i, 1,
						GetAspectByFormat(createInfo.format_),
						currentLayout,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						currentAccess, VK_ACCESS_TRANSFER_WRITE_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT
					);
				}

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

				// Image of previous mip map level as source
				commandBuffer->ImageMemoryBarrier(*image,
					i - 1, 1,
					GetAspectByFormat(createInfo.format_),
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, createInfo.targetLayout_,
					VK_ACCESS_TRANSFER_READ_BIT, createInfo.targetAccess_,
					VK_PIPELINE_STAGE_TRANSFER_BIT, createInfo.targetPipelineStage_
				);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			//Set layout of last mip map level.
			commandBuffer->ImageMemoryBarrier(*image,
				createInfo.mipLevels_ - 1, 1,
				GetAspectByFormat(createInfo.format_),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, createInfo.targetLayout_,
				VK_ACCESS_TRANSFER_WRITE_BIT, createInfo.targetAccess_,
				VK_PIPELINE_STAGE_TRANSFER_BIT, createInfo.targetPipelineStage_
			);

			currentLayout = createInfo.targetLayout_;
			currentAccess = createInfo.targetAccess_;
			currentPipelineStage = createInfo.targetPipelineStage_;

			commandBuffer->End();
			commandBuffer->Submit(createInfo.LD_->GetGraphicsQueue());

			vkQueueWaitIdle(createInfo.LD_->GetGraphicsQueue());

		}

		//Set target layout and access if need.
		if (currentLayout != createInfo.targetLayout_ || currentAccess != createInfo.targetAccess_) {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.name_ = "Set image target layout command buffer";
				commandBufferCreateInfo.LD_ = createInfo.LD_;
				commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
				commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

			commandBuffer->Begin();
			//Set layout of last mip map level.
			commandBuffer->ImageMemoryBarrier(*image,
				createInfo.mipLevels_ - 1, 1,
				GetAspectByFormat(createInfo.format_),
				currentLayout, createInfo.targetLayout_,
				currentAccess, createInfo.targetAccess_,
				currentPipelineStage, createInfo.targetPipelineStage_
			);

			commandBuffer->End();
			commandBuffer->Submit(createInfo.LD_->GetGraphicsQueue());

			vkQueueWaitIdle(createInfo.LD_->GetGraphicsQueue());
		}

		auto imageView = CreateImageViewByImage(createInfo.LD_, image, GetAspectByFormat(createInfo.format_), createInfo.mipLevels_);

		VkDebugUtilsObjectNameInfoEXT nameInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.objectType = VK_OBJECT_TYPE_IMAGE,
			.objectHandle = (uint64_t)image->GetHandle(),
			.pObjectName = createInfo.name_.c_str()
		};
		#if !defined(NDEBUG)
			vkCreateDebugUtilsMessengerEXT(*createInfo.LD_, &nameInfo);
		#endif
	
		Sampler::CreateInfo samplerCreateInfo;
		{
			samplerCreateInfo.LD_ = createInfo.LD_;
			samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
			samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
			samplerCreateInfo.maxAnisotropy_ = createInfo.PD_->GetProperties().limits.maxSamplerAnisotropy;
			samplerCreateInfo.mipLevels_ = (mipMapGenerationEnabled) ? (createInfo.mipLevels_) : (1);
		}
		auto sampler = std::make_shared<Sampler>(samplerCreateInfo);

		ci1_ = createInfo;
		image_ = image;
		imageView_ = imageView;
		sampler_ = sampler;
	}


	void Texture::TextureMemoryBarrier(
		std::shared_ptr<CommandBuffer> commandBuffer,
		VkImageLayout fromLayout,
		VkImageLayout toLayout,
		VkAccessFlags fromAccess,
		VkAccessFlags toAccess,
		VkPipelineStageFlags fromStage,
		VkPipelineStageFlags toStage) {

		commandBuffer->ImageMemoryBarrier(
			GetImage(),
			0,
			ci1_.mipLevels_,
			GetAspectByFormat(ci1_.format_),
			fromLayout,
			toLayout,
			fromAccess, toAccess,
			fromStage, toStage
		);
	}


}