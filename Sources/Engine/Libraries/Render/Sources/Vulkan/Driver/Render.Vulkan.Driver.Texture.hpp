#pragma once 

#include <RAL.Texture.hpp>

#include <Render.Vulkan.Common.hpp>

namespace Render::Vulkan {

	class Texture : public RAL::Texture {
	public:

		struct CreateInfo {
			RAL::Texture::CreateInfo ralCreateInfo_;
			std::shared_ptr<class PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<class LogicDevice> LD_ = nullptr;
			std::shared_ptr<class CommandPool> commandPool_ = nullptr;
			std::shared_ptr<class DescriptorPool> DP_ = nullptr;
			VkFormat format_ = VK_FORMAT_UNDEFINED;
		};

		Texture(const CreateInfo& createInfo);

		[[nodiscard]]
		auto GetDS() noexcept {
			return DS_;
		}

	private:
		std::shared_ptr<class Image> image_ = nullptr;
		std::shared_ptr<class ImageView> imageView_ = nullptr;
		std::shared_ptr<class Sampler> sampler_ = nullptr;
		std::shared_ptr<class DescriptorSetLayout> DSL_ = nullptr;
		std::shared_ptr<class DescriptorSet> DS_ = nullptr;
	};

}