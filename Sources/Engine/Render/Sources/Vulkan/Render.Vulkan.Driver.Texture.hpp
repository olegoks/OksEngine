#pragma once 


#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>

namespace Render::Vulkan {



	class Texture {
	public:
		struct CreateInfo {
			std::shared_ptr<AllocatedTextureImage> textureImage_ = nullptr;
			std::shared_ptr<ImageView> textureImageView_ = nullptr;
			std::shared_ptr<Sampler> sampler = nullptr;
			std::shared_ptr<DescriptorSet> descriptorSet_ = nullptr;
		};

		Texture(const CreateInfo& createInfo) :
			textureImage_{ createInfo.textureImage_ },
			textureImageView_{ createInfo.textureImageView_ },
			sampler{ createInfo.sampler },
			descriptorSet_{ createInfo.descriptorSet_ } {

		}

		[[nodiscard]]
		std::shared_ptr<DescriptorSet> GetDescriptorSet() noexcept {
			return descriptorSet_;
		}

	private:
		std::shared_ptr<AllocatedTextureImage> textureImage_ = nullptr;
		std::shared_ptr<ImageView> textureImageView_ = nullptr;
		std::shared_ptr<Sampler> sampler = nullptr;
		std::shared_ptr<DescriptorSet> descriptorSet_ = nullptr;
	};


}