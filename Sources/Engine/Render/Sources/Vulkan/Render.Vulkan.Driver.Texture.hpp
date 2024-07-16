#pragma once 


#include <Render.Vulkan.Common.hpp>

namespace Render::Vulkan {



	//class Texture {
	//public:
	//	struct CreateInfo {
	//		std::shared_ptr<class AllocatedTextureImage> textureImage_ = nullptr;
	//		std::shared_ptr<class ImageView> textureImageView_ = nullptr;
	//		std::shared_ptr<class Sampler> sampler = nullptr;
	//		std::shared_ptr<class DescriptorSet> descriptorSet_ = nullptr;
	//	};

	//	Texture(const CreateInfo& createInfo) :
	//		textureImage_{ createInfo.textureImage_ },
	//		textureImageView_{ createInfo.textureImageView_ },
	//		sampler{ createInfo.sampler },
	//		descriptorSet_{ createInfo.descriptorSet_ } {

	//	}

	//	[[nodiscard]]
	//	std::shared_ptr<DescriptorSet> GetDescriptorSet() noexcept {
	//		return descriptorSet_;
	//	}

	//private:
	//	std::shared_ptr<class AllocatedTextureImage> textureImage_ = nullptr;
	//	std::shared_ptr<class ImageView> textureImageView_ = nullptr;
	//	std::shared_ptr<class Sampler> sampler = nullptr;
	//	std::shared_ptr<class DescriptorSet> descriptorSet_ = nullptr;
	//};


	class VkTexture {
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

		VkTexture(const CreateInfo& createInfo);

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