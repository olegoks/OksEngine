#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class DescriptorPool : public Abstraction<VkDescriptorPool> {
	public:
		DescriptorPool(std::shared_ptr<LogicDevice> logicDevice, Common::Size size) {
			VkDescriptorPoolSize poolSize{};
			{
				poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				poolSize.descriptorCount = static_cast<uint32_t>(size);
			}
			//VkDescriptorPoolSize poolSizes[] =
			//{
			//	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			//	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			//};
			VkDescriptorPoolCreateInfo poolInfo{};
			{
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.poolSizeCount = 1;
				poolInfo.pPoolSizes = &poolSize;
				poolInfo.maxSets = static_cast<uint32_t>(size);
			}
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
			if (vkCreateDescriptorPool(logicDevice->GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}
			SetHandle(descriptorPool);
		}
		~DescriptorPool() {
			vkDestroyDescriptorPool(logicDevice_->GetHandle(), GetHandle(), nullptr);
		}
	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
	};
}