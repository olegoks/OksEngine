#pragma once 

#include <RAL.Texture.hpp>

#include <Render.Vulkan.Common.hpp>

namespace Render::Vulkan {

	class Texture : public RAL::Texture {
	public:

		struct CreateInfo1 {
			RAL::Texture::CreateInfo ralCreateInfo_;
			std::shared_ptr<class PhysicalDevice> PD_ = nullptr;
			std::shared_ptr<class LogicDevice> LD_ = nullptr;
			std::shared_ptr<class CommandPool> commandPool_ = nullptr;
			//std::shared_ptr<class DescriptorPool> DP_ = nullptr;
			Common::UInt32 mipLevels_ = 1;
			VkFormat format_ = VK_FORMAT_UNDEFINED;
		};

		Texture(const CreateInfo1& createInfo);

		//[[nodiscard]]
		//auto GetDS() noexcept {
		//	return DS_;
		//}

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

	private:
		std::shared_ptr<class Image> image_ = nullptr;
		std::shared_ptr<class ImageView> imageView_ = nullptr;
		std::shared_ptr<class Sampler> sampler_ = nullptr;
		//std::shared_ptr<class DescriptorSetLayout> DSL_ = nullptr;
		//std::shared_ptr<class DescriptorSet> DS_ = nullptr;
	};

}