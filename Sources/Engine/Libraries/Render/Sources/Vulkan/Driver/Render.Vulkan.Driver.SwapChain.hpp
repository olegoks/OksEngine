#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

//#include <UIAL.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.WindowSurface.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>



namespace Render::Vulkan {

	class SwapChain final : public Abstraction<VkSwapchainKHR> {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			std::shared_ptr<WindowSurface> windowSurface_;
			QueueFamily presentQueueFamily_;
			QueueFamily graphicsQueueFamily_;
			glm::u32vec2 imageSize_ = { 0, 0 };

			VkSurfaceFormatKHR format_;
			VkPresentModeKHR presentMode_;
			VkSurfaceCapabilitiesKHR capabilities_;
			VkExtent2D extent_;
		};

		using Images = std::vector<VkImage>;
		using ImageViews = std::vector<std::shared_ptr<ImageView>>;

		SwapChain(const CreateInfo& createInfo) noexcept :
			createInfo_{ createInfo },
			imagesNumber_{ CalculateImagesNumber(createInfo.capabilities_) } {

			OS::AssertMessage(createInfo.imageSize_.length() != 0, "Please, set correct swap chain images size.");

			VkSwapchainCreateInfoKHR swapChainCreateInfo{};
			{
				swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapChainCreateInfo.surface = createInfo.windowSurface_->GetHandle();
				swapChainCreateInfo.minImageCount = GetImagesNumber();
				swapChainCreateInfo.imageFormat = createInfo.format_.format;
				swapChainCreateInfo.imageColorSpace = createInfo.format_.colorSpace;
				swapChainCreateInfo.imageExtent = createInfo.extent_;
				swapChainCreateInfo.imageArrayLayers = 1;
				swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				std::array<QueueFamily::Index, 2> indices{ createInfo.graphicsQueueFamily_.index_, createInfo.presentQueueFamily_.index_ };
				{
					if (createInfo.graphicsQueueFamily_.index_ == createInfo.presentQueueFamily_.index_) {
						swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
						swapChainCreateInfo.queueFamilyIndexCount = 0;
						swapChainCreateInfo.pQueueFamilyIndices = nullptr;
					} else {
						swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
						swapChainCreateInfo.queueFamilyIndexCount = static_cast<Common::UInt32>(indices.size());
						swapChainCreateInfo.pQueueFamilyIndices = indices.data();
					}
				}

				swapChainCreateInfo.preTransform = createInfo.capabilities_.currentTransform;
				swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				swapChainCreateInfo.presentMode = createInfo.presentMode_;
				swapChainCreateInfo.clipped = VK_TRUE;
				swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; //if window was resized.

				VkSwapchainKHR swapChain = VK_NULL_HANDLE;
				VkCall(vkCreateSwapchainKHR(createInfo.LD_->GetHandle(), &swapChainCreateInfo, nullptr, &swapChain),
					"Error while creating swap chain.");

				SetHandle(swapChain);
			}

			images_ = GetImages(createInfo.LD_);

			{
				ImageViews imageViews;
				for (VkImage image : images_) {
					auto imageView = CreateImageViewByImage(createInfo.LD_, image, createInfo.format_.format, VK_IMAGE_ASPECT_COLOR_BIT);
					imageViews.push_back(imageView);
				}
				imageViews_ = std::move(imageViews);
			}
		}

		[[nodiscard]]
		const Images& GetImages() const noexcept { return images_; }
		
		[[nodiscard]]
		const ImageViews& GetImageViews() const { return imageViews_; }

		~SwapChain() noexcept {
			
			Destroy();
		}

		[[nodiscard]]
		VkExtent2D GetExtent() const noexcept {
			return createInfo_.extent_;
		}

		[[nodiscard]]
		glm::u32vec2 GetSize() const noexcept {
			return { createInfo_.extent_.width, createInfo_.extent_.height };
		}

		[[nodiscard]]
		VkSurfaceFormatKHR GetFormat() const noexcept {
			return createInfo_.format_;
		}

		[[nodiscard]]
		VkPresentModeKHR GetPresentMode() const noexcept {
			return createInfo_.presentMode_;
		}

		[[nodiscard]]
		uint32_t GetImagesNumber() const noexcept {
			OS::AssertMessage(imagesNumber_ != 0, "Image count can't be zero.");
			return imagesNumber_;
		}

	private:

		[[nodiscard]]
		Images GetImages(std::shared_ptr<LogicDevice> logicDevice) const noexcept {

			Images images;
			uint32_t imageCount = 0;
			vkGetSwapchainImagesKHR(logicDevice->GetHandle(), GetHandle(), &imageCount, nullptr);
			images.resize(imageCount);
			vkGetSwapchainImagesKHR(logicDevice->GetHandle(), GetHandle(), &imageCount, images.data());

			return images;
		}

		[[nodiscard]]
		uint32_t CalculateImagesNumber(VkSurfaceCapabilitiesKHR capabilities) noexcept {
			const uint32_t imageCount = capabilities.minImageCount + 1;
			if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
				return capabilities.maxImageCount;
			}
			return imageCount;
		}


		void Destroy() noexcept {
			vkDestroySwapchainKHR(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		CreateInfo createInfo_{ 0 };
		const uint32_t imagesNumber_ = 0;
		Images images_;
		ImageViews imageViews_;

	};

	[[nodiscard]]
	inline VkSurfaceFormatKHR ChooseSwapChainSurfaceFormat(const PhysicalDevice::Formats availableFormats) noexcept {

		VkSurfaceFormatKHR format;

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		format = availableFormats[0];

		return format;
	}


	[[nodiscard]]
	inline VkSurfaceCapabilitiesKHR GetCapabilities(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->GetHandle(), windowSurface->GetHandle(), &capabilities);

		return capabilities;
	}

	[[nodiscard]]
	inline VkExtent2D ChooseSwapExtent(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface, const glm::u32vec2& imageSize) noexcept {

		const VkSurfaceCapabilitiesKHR capabilities = GetCapabilities(physicalDevice, windowSurface);

		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(imageSize.x),
				static_cast<uint32_t>(imageSize.y)
			};
#undef max
#undef min
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

}