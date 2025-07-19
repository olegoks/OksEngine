#pragma once 

#include <Render.Vulkan.Common.hpp>

namespace Render::Vulkan {



	class Texture {
	public:

		struct CreateInfo1 {
			std::string name_ = "No name";
			VkFormat format_ = VkFormat::VK_FORMAT_MAX_ENUM;
			std::vector<Common::Byte> data_;
			glm::u32vec2 size_{ 0, 0 };
			VkImageLayout targetLayout_ = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
			VkAccessFlags targetAccess_ = VkAccessFlagBits::VK_ACCESS_FLAG_BITS_MAX_ENUM;
			VkPipelineStageFlags targetPipelineStage_ = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
			VkImageUsageFlags usages_ = VkImageUsageFlagBits::VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
			Common::UInt32 mipLevels_ = 0;
			std::shared_ptr<class PhysicalDevice> PD_ = nullptr;
			std::shared_ptr<class LogicDevice> LD_ = nullptr;
			std::shared_ptr<class CommandPool> commandPool_ = nullptr;
		};
		using CI1 = CreateInfo1;

		Texture(const CreateInfo1& createInfo);

		[[nodiscard]]
		auto GetSampler() {
			return sampler_;
		}
		[[nodiscard]]
		auto GetImageView() {
			return imageView_;
		}

		[[nodiscard]]
		auto GetImage() {
			return image_;
		}

		Common::UInt32 GetWidth() const noexcept {
			return ci1_.size_.x;
		}

		Common::UInt32 GetHeight() const noexcept {
			return ci1_.size_.y;
		}

	private:
		CI1 ci1_;
		std::shared_ptr<class Image> image_ = nullptr;
		std::shared_ptr<class ImageView> imageView_ = nullptr;
		std::shared_ptr<class Sampler> sampler_ = nullptr;
	};

}