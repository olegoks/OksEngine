#pragma once

#include <memory>
#include <array>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class DescriptorPool {
	public:
		DescriptorPool(std::shared_ptr<LogicDevice> logicDevice, Common::Size size) {

			VkDescriptorPoolSize poolSizes[] = {
				{
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					static_cast<uint32_t>(size + 1000)
				},
				{
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					static_cast<uint32_t>(size + 1000)
				},
				{
					VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
					static_cast<uint32_t>(10)
				}

			};

			VkDescriptorPoolCreateInfo poolInfo{};
			{
				poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
				poolInfo.pPoolSizes = &poolSizes[0];
				poolInfo.maxSets = static_cast<uint32_t>(size) + 1000;
				poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //If individual descriptor sets can be freed or not
			}
			if (vkCreateDescriptorPool(logicDevice->GetHandle(), &poolInfo, nullptr, &DP_) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}
		~DescriptorPool() {
			vkDestroyDescriptorPool(LD_->GetHandle(), DP_, nullptr);
		}
		VkDescriptorPool GetNative() const { return DP_; }

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
		VkDescriptorPool DP_;
	};

	using DP = DescriptorPool;
}