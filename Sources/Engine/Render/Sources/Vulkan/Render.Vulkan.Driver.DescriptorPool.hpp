#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.DescriptorPool;

import OksEngine.Common.Types;

import OksEngine.OS.Assert;
import OksEngine.OS.Logger;

import OksEngine.Render.Vulkan.Common;
import OksEngine.Render.Vulkan.Driver.LogicDevice;

namespace Render::Vulkan {

	class DescriptorPool {
	public:
		DescriptorPool(std::shared_ptr<LogicDevice> logicDevice, Common::Size size) {
			VkDescriptorPoolSize poolSize{};
			{
				poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				poolSize.descriptorCount = static_cast<uint32_t>(size);
			}
			VkDescriptorPoolCreateInfo poolInfo{};
			{
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.poolSizeCount = 1;
				poolInfo.pPoolSizes = &poolSize;
				poolInfo.maxSets = static_cast<uint32_t>(size);
			}
			if (vkCreateDescriptorPool(logicDevice->GetHandle(), &poolInfo, nullptr, &descriptorPool_) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}
		~DescriptorPool() {
			vkDestroyDescriptorPool(logicDevice_->GetHandle(), descriptorPool_, nullptr);
		}
		VkDescriptorPool GetNative() const { return descriptorPool_; }

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		VkDescriptorPool descriptorPool_;
	};
}