#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <Math.Vector.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.DeviceMemory.hpp>
#include <Common.Types.hpp>

namespace Render::Vulkan {

	class Image : public Abstraction<VkImage> {
	public:

		struct CreateInfo {
            std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
            std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
            VkExtent2D extent_ = { 0, 0 };
            VkFormat format_ = VK_FORMAT_MAX_ENUM;
            VkImageTiling tiling_ = VK_IMAGE_TILING_MAX_ENUM;
            VkImageUsageFlags usage_ = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
		};

        Image(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

            OS::AssertMessage(createInfo.extent_.height != 0 && createInfo.extent_.width != 0, "Incorrect image extent.");
            OS::AssertMessage(createInfo.format_ != VK_FORMAT_MAX_ENUM, "Incorrect image format.");
            OS::AssertMessage(createInfo.tiling_ != VK_IMAGE_TILING_MAX_ENUM, "Incorrect image tiling.");
            OS::AssertMessage(createInfo.usage_ != VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM, "Incorrect image usage.");
            OS::AssertMessage(createInfo.logicDevice_ != nullptr, "Logic device is not set.");

            VkImageCreateInfo imageInfo{};
            {
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType = VK_IMAGE_TYPE_2D;
                imageInfo.extent.height = createInfo.extent_.height;
                imageInfo.extent.width = createInfo.extent_.width;
                imageInfo.extent.depth = 1;
                imageInfo.mipLevels = 1;
                imageInfo.arrayLayers = 1;
                imageInfo.format = createInfo.format_;
                imageInfo.tiling = createInfo.tiling_;
                imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageInfo.usage = createInfo.usage_;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }

            VkImage image = VK_NULL_HANDLE;
            VkCall(vkCreateImage(createInfo.logicDevice_->GetHandle(), &imageInfo, nullptr, &image),
                "Errow while creating image.");
            SetHandle(image);
		}

        void BindMemory(std::shared_ptr<DeviceMemory> deviceMemory) noexcept {
            VkCall(vkBindImageMemory(createInfo_.logicDevice_->GetHandle(), GetHandle(), deviceMemory->GetHandle(), 0),
                "Error while binding device memory to image.");
        }

        [[nodiscard]]
        VkMemoryRequirements GetMemoryRequirements() const noexcept {
            VkMemoryRequirements memRequirements = { 0 };
            vkGetImageMemoryRequirements(*createInfo_.logicDevice_, GetHandle(), &memRequirements);
            return memRequirements;
        }

        Image(const Image& image)noexcept = delete;

        [[nodiscard]]
        VkFormat GetFormat() const noexcept { return createInfo_.format_; }

        ~Image() noexcept {
            Destroy();
        }

    private:

        void Destroy() noexcept {
            vkDestroyImage(createInfo_.logicDevice_->GetHandle(), GetHandle(), nullptr);
        }

	private:
        CreateInfo createInfo_{ 0 };
	};

    class AllocatedImage final : public Image{
    public:

        AllocatedImage(const CreateInfo& createInfo) :
    		Image{ createInfo } {

            const VkMemoryRequirements depthImageMemoryRequirements = GetMemoryRequirements();

            DeviceMemory::CreateInfo deviceMemoryCreateInfo;
            {
                deviceMemoryCreateInfo.logicDevice_ = createInfo.logicDevice_;
                deviceMemoryCreateInfo.requirements_ = depthImageMemoryRequirements;
                deviceMemoryCreateInfo.memoryTypeIndex_ = createInfo.physicalDevice_->GetSuitableMemoryTypeIndex(depthImageMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            }
            deviceMemory_ = std::make_shared<DeviceMemory>(deviceMemoryCreateInfo);

            BindMemory(deviceMemory_);

        }

    private:
        std::shared_ptr<DeviceMemory> deviceMemory_ = nullptr;
        using Image::BindMemory;
        using Image::GetMemoryRequirements;
    };

}