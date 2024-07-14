#pragma once 

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class Sampler : public Abstraction<VkSampler>{
	public:
		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			VkFilter magFilter_ = VK_FILTER_MAX_ENUM;
			VkFilter minFilter_ = VK_FILTER_MAX_ENUM;
			float maxAnisotropy_ = 0.0f;
		};


		Sampler(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {

			VkSamplerCreateInfo samplerCreateInfo{};
			{
				samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerCreateInfo.magFilter = createInfo.magFilter_;
				samplerCreateInfo.minFilter = createInfo.minFilter_;
				samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.anisotropyEnable = VK_TRUE;
				samplerCreateInfo.maxAnisotropy = createInfo.maxAnisotropy_;
				samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
				samplerCreateInfo.compareEnable = VK_FALSE;
				samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
				samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				samplerCreateInfo.mipLodBias = 0.0f;
				samplerCreateInfo.minLod = -1000.0f;
				samplerCreateInfo.maxLod = 1000.0f;
				//samplerCreateInfo.flags = 0;
				VkSampler sampler_ = VK_NULL_HANDLE;
				VkCall(vkCreateSampler(*createInfo.logicDevice_, &samplerCreateInfo, nullptr, &sampler_), 
					"Error while creating sampler.");
				SetHandle(sampler_);
			}

		}

		~Sampler() {
			OS::Assert(GetHandle() != VK_NULL_HANDLE);
			vkDestroySampler(*createInfo_.logicDevice_, GetHandle(), nullptr);
		}
	private:
		CreateInfo createInfo_;
	};

}