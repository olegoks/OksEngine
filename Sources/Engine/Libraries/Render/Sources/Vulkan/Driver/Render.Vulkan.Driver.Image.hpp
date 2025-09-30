#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <Math.Vector.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.DeviceMemory.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Common.Types.hpp>

namespace Render::Vulkan {

	class Image : public Abstraction<VkImage> {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::string name_ = "Image";
			glm::u32vec2 size_ = { 0, 0 };
			VkFormat format_ = VK_FORMAT_MAX_ENUM;
			VkImageTiling tiling_ = VK_IMAGE_TILING_MAX_ENUM;
			VkImageUsageFlags usage_ = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
			VkSampleCountFlagBits samplesCount_ = VK_SAMPLE_COUNT_1_BIT;
			Common::UInt32 mipLevels_ = 1;
		};

		Image(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

			ASSERT_FMSG(createInfo.size_.length() != 0, "Incorrect image extent.");
			ASSERT_FMSG(createInfo.format_ != VK_FORMAT_MAX_ENUM, "Incorrect image format.");
			ASSERT_FMSG(createInfo.tiling_ != VK_IMAGE_TILING_MAX_ENUM, "Incorrect image tiling.");
			ASSERT_FMSG(createInfo.usage_ != VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM, "Incorrect image usage.");
			ASSERT_FMSG(createInfo.LD_ != nullptr, "Logic device is not set.");

			VkImageCreateInfo imageInfo{};
			{
				imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageInfo.imageType = VK_IMAGE_TYPE_2D;
				imageInfo.extent.height = createInfo.size_.y;
				imageInfo.extent.width = createInfo.size_.x;
				imageInfo.extent.depth = 1;
				imageInfo.mipLevels = createInfo.mipLevels_;
				imageInfo.arrayLayers = 1;
				imageInfo.format = createInfo.format_;
				imageInfo.flags = 0;
				imageInfo.tiling = createInfo.tiling_;
				imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageInfo.usage = createInfo.usage_;
				imageInfo.samples = createInfo.samplesCount_;
				imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			}

			VkImage image = VK_NULL_HANDLE;
			VK_CALL(vkCreateImage(createInfo.LD_->GetHandle(), &imageInfo, nullptr, &image),
				"Errow while creating image.");
			VkDebugUtilsObjectNameInfoEXT nameInfo = {};
			nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			nameInfo.objectType = VK_OBJECT_TYPE_IMAGE; // Тип объекта,, VK_OBJECT_TYPE_BUFFER
			nameInfo.objectHandle = (uint64_t)image;   // Объект, которому назначается имя
			nameInfo.pObjectName = createInfo.name_.c_str();      // Имя объекта
			SetObjectName(*createInfo.LD_, &nameInfo);
			SetHandle(image);
		}

		void BindMemory(std::shared_ptr<DeviceMemory> deviceMemory) noexcept {
			VK_CALL(vkBindImageMemory(createInfo_.LD_->GetHandle(), GetHandle(), deviceMemory->GetHandle(), 0),
				"Error while binding device memory to image. Maybe double memory binding.");
		}

		[[nodiscard]]
		VkMemoryRequirements GetMemoryRequirements() const noexcept {
			VkMemoryRequirements memRequirements = { 0 };
			vkGetImageMemoryRequirements(*createInfo_.LD_, GetHandle(), &memRequirements);
			return memRequirements;
		}

		Image(const Image& image)noexcept = delete;

		[[nodiscard]]
		VkFormat GetFormat() const noexcept { return createInfo_.format_; }

		[[nodiscard]]
		glm::u32vec2 GetSize() const noexcept {
			return createInfo_.size_;
		}


		//void ChangeLayout(
		//	VkImageLayout oldLayout,
		//	VkImageLayout newLayout,
		//	std::shared_ptr<class CommandPool> commandPool);



		static void DataCopy(
			std::shared_ptr<Buffer> bufferFrom,
			std::shared_ptr<Image> imageTo,
			std::shared_ptr<LogicDevice> logicDevice,
			std::shared_ptr<class CommandPool> commandPool);

		virtual ~Image() noexcept;


		void Destroy() noexcept;

	private:
		CreateInfo createInfo_{ 0 };
	};

	class TextureImage : public Image {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::string name_;
			glm::u32vec2 size_;
			VkFormat format_;
			VkImageUsageFlags usage_ = VkImageUsageFlagBits::VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
			Common::UInt32 mipLevels_ = 1;
		};

		TextureImage(const CreateInfo& createInfo) :
			Image{ Image::CreateInfo{
			createInfo.physicalDevice_,
			createInfo.LD_,
			createInfo.name_,
			createInfo.size_,
			createInfo.format_,
			VK_IMAGE_TILING_OPTIMAL,
			createInfo.usage_,//VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_SAMPLE_COUNT_1_BIT,
			createInfo.mipLevels_ }
		} { }

	private:
	};

	class DepthImage : public Image {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::string name_;
			glm::u32vec2 size_{ 0, 0 };
		};
		DepthImage(const CreateInfo& createInfo) :
			Image{ Image::CreateInfo{
						createInfo.physicalDevice_,
						createInfo.LD_,
						createInfo.name_,
						createInfo.size_,
						VK_FORMAT_D32_SFLOAT,
						VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					} } { }
	};


	class AllocatedImage final : public Image {
	public:

		AllocatedImage(const Image::CreateInfo& createInfo) :
			Image{ createInfo } {

			const VkMemoryRequirements depthImageMemoryRequirements = GetMemoryRequirements();

			DeviceMemory::CreateInfo deviceMemoryCreateInfo;
			{
				deviceMemoryCreateInfo.LD_ = createInfo.LD_;
				deviceMemoryCreateInfo.requirements_ = depthImageMemoryRequirements;
				deviceMemoryCreateInfo.memoryTypeIndex_ = createInfo.physicalDevice_->GetSuitableMemoryTypeIndex(depthImageMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}
			deviceMemory_ = std::make_shared<DeviceMemory>(deviceMemoryCreateInfo);

			BindMemory(deviceMemory_);

		}

		~AllocatedImage() override {
			//Hack to destroy image before device memory destruction.
			Image::Destroy();
		}

	private:
		std::shared_ptr<DeviceMemory> deviceMemory_ = nullptr;
		using Image::BindMemory;
		using Image::GetMemoryRequirements;
	};

	class AllocatedDepthImage : public DepthImage {
	public:
		AllocatedDepthImage(const CreateInfo& createInfo) noexcept :
			DepthImage{ createInfo }
		{
			const VkMemoryRequirements depthImageMemoryRequirements = GetMemoryRequirements();

			DeviceMemory::CreateInfo deviceMemoryCreateInfo;
			{
				deviceMemoryCreateInfo.LD_ = createInfo.LD_;
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

	class AllocatedTextureImage : public TextureImage {
	public:
		AllocatedTextureImage(const CreateInfo& createInfo) noexcept :
			TextureImage{ createInfo }
		{
			const VkMemoryRequirements depthImageMemoryRequirements = GetMemoryRequirements();

			DeviceMemory::CreateInfo deviceMemoryCreateInfo;
			{
				deviceMemoryCreateInfo.LD_ = createInfo.LD_;
				deviceMemoryCreateInfo.requirements_ = depthImageMemoryRequirements;
				deviceMemoryCreateInfo.memoryTypeIndex_ = createInfo.physicalDevice_->GetSuitableMemoryTypeIndex(depthImageMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}
			deviceMemory_ = std::make_shared<DeviceMemory>(deviceMemoryCreateInfo);

			BindMemory(deviceMemory_);
		}

		~AllocatedTextureImage()
		{
			//Hack to destroy image before device memory destruction.
			TextureImage::Destroy();
		}
	private:
		std::shared_ptr<DeviceMemory> deviceMemory_ = nullptr;
		using Image::BindMemory;
		using Image::GetMemoryRequirements;
	};

}