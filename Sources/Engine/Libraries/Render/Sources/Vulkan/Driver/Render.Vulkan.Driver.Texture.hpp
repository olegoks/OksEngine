#pragma once 

#include <RAL.Texture.hpp>

#include <Render.Vulkan.Common.hpp>

namespace Render::Vulkan {

	class Texture : public RAL::Texture {
	public:

		struct CreateInfo {
			std::string name_ = "No name";
			std::shared_ptr<class PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<class LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<class CommandPool> commandPool_ = nullptr;
			std::shared_ptr<class DescriptorPool> descriptorPool_ = nullptr;
			glm::u32vec2 size_{ 0, 0 };
			VkFormat format_ = VK_FORMAT_UNDEFINED;
			const void* pixels_ = nullptr;

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