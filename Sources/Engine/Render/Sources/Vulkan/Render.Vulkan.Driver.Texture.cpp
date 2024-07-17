
#include <Render.Vulkan.Driver.Texture.hpp>

#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>


namespace Render::Vulkan {


	Texture::Texture(const CreateInfo& createInfo) {
			OS::Assert(createInfo.format_ == VK_FORMAT_R8G8B8A8_UNORM);
			auto textureStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.size_.x * createInfo.size_.y * sizeof(RAL::Color4b));
			textureStagingBuffer->Fill(createInfo.pixels_);

			AllocatedTextureImage::CreateInfo textureImageCreateInfo;
			{
				textureImageCreateInfo.size_ = createInfo.size_;
				textureImageCreateInfo.format_ = createInfo.format_;
				textureImageCreateInfo.logicDevice_ = createInfo.logicDevice_;
				textureImageCreateInfo.physicalDevice_ = createInfo.physicalDevice_;
			}
			auto image = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);
			image->ChangeLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, createInfo.commandPool_);
			Image::DataCopy(textureStagingBuffer, image, createInfo.logicDevice_, createInfo.commandPool_);
			image->ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, createInfo.commandPool_);
			auto imageView = CreateImageViewByImage(createInfo.logicDevice_, image, VK_IMAGE_ASPECT_COLOR_BIT);

			Sampler::CreateInfo samplerCreateInfo;
			{
				samplerCreateInfo.logicDevice_ = createInfo.logicDevice_;
				samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.maxAnisotropy_ = createInfo.physicalDevice_->GetProperties().limits.maxSamplerAnisotropy;
			}
			auto sampler = std::make_shared<Sampler>(samplerCreateInfo);

			auto DSL = std::make_shared<DescriptorSetLayout>(
				DescriptorSetLayout::CreateInfo{
					"DiffuseMap",
					createInfo.logicDevice_,
					std::vector<VkDescriptorSetLayoutBinding>{{
						{
					0,
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					1,
					VK_SHADER_STAGE_FRAGMENT_BIT,
					nullptr
					}
					}}
				});

			DescriptorSet::CreateInfo DSCreateInfo;
			{
				DSCreateInfo.descriptorPool_ = createInfo.descriptorPool_;
				DSCreateInfo.DSL_ = DSL;
				DSCreateInfo.logicDevice_ = createInfo.logicDevice_;
			}

			auto DS = std::make_shared<DescriptorSet>(DSCreateInfo);
			DS->UpdateImageWriteConfiguration(
				imageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				sampler,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				0);

			image_ = image;
			imageView_ = imageView;
			sampler_ = sampler;
			DSL_ = DSL;
			DS_ = DS;

		}

}