#pragma once 

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.Image.hpp>

namespace Render::Vulkan {

	class ImageView final : public Abstraction<VkImageView> {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<Image> image_ = nullptr;
			VkFormat format_ = VK_FORMAT_MAX_ENUM;
			VkImageAspectFlags aspectFlags_ = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;//VK_IMAGE_ASPECT_COLOR_BIT;
		};

		struct CreateInfo2 {
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			VkImage imageHandle_ = VK_NULL_HANDLE;
			VkFormat format_ = VK_FORMAT_MAX_ENUM;
			VkImageAspectFlags aspectFlags_ = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;//VK_IMAGE_ASPECT_COLOR_BIT;
		};

		ImageView(ImageView&& moveImageView) noexcept : createInfo_{ 0 } {
		
			std::swap(createInfo_, moveImageView.createInfo_);
		
		}

		ImageView(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {
		
			OS::AssertMessage(createInfo.logicDevice_ != nullptr, "");
			OS::AssertMessage(createInfo.format_ != VK_FORMAT_MAX_ENUM, "");
			OS::AssertMessage(createInfo.aspectFlags_ != VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM, "Aspect flags were not set for image view.");

			VkImageViewCreateInfo vkCreateInfo{};
			{
				vkCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vkCreateInfo.image = createInfo.image_->GetHandle();
				vkCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				vkCreateInfo.format = createInfo.format_;
				vkCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.subresourceRange.aspectMask = createInfo.aspectFlags_;
				vkCreateInfo.subresourceRange.baseMipLevel = 0;
				vkCreateInfo.subresourceRange.levelCount = 1;
				vkCreateInfo.subresourceRange.baseArrayLayer = 0;
				vkCreateInfo.subresourceRange.layerCount = 1;
			}

			VkImageView imageView = VK_NULL_HANDLE;
			VkCall(vkCreateImageView(createInfo.logicDevice_->GetHandle(), &vkCreateInfo, nullptr, &imageView), 
				"Error while creating image view.");
			SetHandle(imageView);
		}

		ImageView(const CreateInfo2& createInfo) noexcept : createInfo_{ 0 } {

			OS::AssertMessage(createInfo.logicDevice_ != nullptr, "");
			OS::AssertMessage(createInfo.format_ != VK_FORMAT_MAX_ENUM, "");
			OS::AssertMessage(createInfo.aspectFlags_ != VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM, "Aspect flags were not set for image view.");

			createInfo_.format_ = createInfo.format_;
			createInfo_.aspectFlags_ = createInfo.aspectFlags_;
			createInfo_.logicDevice_ = createInfo.logicDevice_;

			VkImageViewCreateInfo vkCreateInfo{};
			{
				vkCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				vkCreateInfo.image = createInfo.imageHandle_;
				vkCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				vkCreateInfo.format = createInfo.format_;
				vkCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				vkCreateInfo.subresourceRange.aspectMask = createInfo.aspectFlags_;
				vkCreateInfo.subresourceRange.baseMipLevel = 0;
				vkCreateInfo.subresourceRange.levelCount = 1;
				vkCreateInfo.subresourceRange.baseArrayLayer = 0;
				vkCreateInfo.subresourceRange.layerCount = 1;
			}

			VkImageView imageView = VK_NULL_HANDLE;
			VkCall(vkCreateImageView(createInfo.logicDevice_->GetHandle(), &vkCreateInfo, nullptr, &imageView),
				"Error while creating image view.");
			SetHandle(imageView);
		}

		~ImageView() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(createInfo_.logicDevice_ != nullptr, "Logic device is not set.");
			vkDestroyImageView(createInfo_.logicDevice_->GetHandle(), GetHandle(), nullptr);
			SetHandle(VK_NULL_HANDLE);
		}

	private:
		CreateInfo createInfo_{ 0 };
	};

	[[nodiscard]]
	inline std::shared_ptr<ImageView> CreateImageViewByImage(std::shared_ptr<LogicDevice> logicDevice, std::shared_ptr<Image> image, VkImageAspectFlags aspect) noexcept {
		ImageView::CreateInfo imageViewCreateInfo;
		{
			imageViewCreateInfo.logicDevice_ = logicDevice;
			imageViewCreateInfo.format_ = image->GetFormat();
			imageViewCreateInfo.image_ = image;
			imageViewCreateInfo.aspectFlags_ = aspect;
		}

		return std::make_shared<ImageView>(imageViewCreateInfo);
	}

	[[deprecated]]
	[[nodiscard]]
	inline std::shared_ptr<ImageView> CreateImageViewByImage(std::shared_ptr<LogicDevice> logicDevice, VkImage image, VkFormat format, VkImageAspectFlags aspect) noexcept {
		ImageView::CreateInfo2 imageViewCreateInfo;
		{
			imageViewCreateInfo.logicDevice_ = logicDevice;
			imageViewCreateInfo.format_ = format;
			imageViewCreateInfo.imageHandle_ = image;
			imageViewCreateInfo.aspectFlags_ = aspect;
		}

		return std::make_shared<ImageView>(imageViewCreateInfo);
	}

}