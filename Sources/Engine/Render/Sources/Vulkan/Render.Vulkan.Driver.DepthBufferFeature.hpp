#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>


export module OksEngine.Render.Vulkan.Driver.DepthBufferFeature;

import OksEngine.OS;
import OksEngine.Render.Vulkan.Driver.LogicDevice;
import OksEngine.Render.Vulkan.Driver.PhysicalDevice;
import OksEngine.Render.Vulkan.Driver.SwapChain;
import OksEngine.Render.Vulkan.Driver.ImageView;
import OksEngine.Render.Vulkan.Driver.Image;
import OksEngine.Render.Vulkan.Driver.DeviceMemory;

namespace Render::Vulkan {


	class DepthBufferFeature {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;

		};

		DepthBufferFeature(const CreateInfo& createInfo) noexcept :
			logicDevice_{ createInfo.logicDevice_ },
			physicalDevice_{ createInfo.physicalDevice_ } {

			const VkFormat depthImageFormat = findSupportedFormat();
			depthImageFormat_ = depthImageFormat;
			Image::CreateInfo imageCreateInfo;
			{
				imageCreateInfo.format_ = depthImageFormat;
				imageCreateInfo.logicDevice_ = createInfo.logicDevice_;
				imageCreateInfo.extent_ = createInfo.swapChain_->GetExtent();
				imageCreateInfo.tiling_ = depthImageTiling_;
				imageCreateInfo.usage_ = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			depthImage_ = std::make_shared<Image>(imageCreateInfo);
			ImageView::CreateInfo imageViewCreateInfo;
			{
				imageViewCreateInfo.logicDevice_ = createInfo.logicDevice_;
				imageViewCreateInfo.format_ = depthImageFormat;
				imageViewCreateInfo.aspectFlags_ = VK_IMAGE_ASPECT_DEPTH_BIT;
				imageViewCreateInfo.image_ = depthImage_;
			}
			depthImageView_ = std::make_shared<ImageView>(imageViewCreateInfo);

		}

		[[nodiscard]]
		std::shared_ptr<ImageView> GetImageView() noexcept { return depthImageView_; }

		[[nodiscard]]
		VkFormat GetDepthImageFormat() const noexcept { return depthImageFormat_; }

	private:

		inline static const VkFormat depthImageFormats_[] = {
			
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT
		};

		inline static const VkImageTiling depthImageTiling_ = VK_IMAGE_TILING_OPTIMAL;
		inline static const VkFormatFeatureFlags depthFormatFeature_ = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

		[[nodiscard]]
		VkFormat findSupportedFormat() {

			for (VkFormat format : depthImageFormats_) {
				const VkFormatProperties formatProperties = physicalDevice_->GetFormatProperties(format);
				if (formatProperties.optimalTilingFeatures & depthFormatFeature_) {
					return format;
				}
			}

			OS::AssertFailMessage("Supported format for depth image was not found.");
			return VkFormat::VK_FORMAT_UNDEFINED;
		}

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
		VkFormat depthImageFormat_ = VK_FORMAT_UNDEFINED;
		std::shared_ptr<Image> depthImage_ = VK_NULL_HANDLE;
		std::shared_ptr<DeviceMemory> depthImageMemory_ = VK_NULL_HANDLE;
		std::shared_ptr<ImageView> depthImageView_ = VK_NULL_HANDLE;
	};

};