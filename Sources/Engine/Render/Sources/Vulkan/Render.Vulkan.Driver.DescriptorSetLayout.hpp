#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Common.Types.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class DescriptorSetLayout {
	public:
		DescriptorSetLayout(std::shared_ptr<LogicDevice> logicDevice) :
			logicDevice_{ logicDevice } {
			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			{
				uboLayoutBinding.binding = 0;
				uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uboLayoutBinding.descriptorCount = 1;
				uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
			}
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			{
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = 1;
				layoutInfo.pBindings = &uboLayoutBinding;
			}
			const VkResult result = vkCreateDescriptorSetLayout(logicDevice->GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout_);
			OS::AssertMessage(result == VK_SUCCESS, "Error while creating descriptor set layout.");
		}
		~DescriptorSetLayout() {
			vkDestroyDescriptorSetLayout(logicDevice_->GetHandle(), descriptorSetLayout_, nullptr);
		}

		VkDescriptorSetLayout& GetNative() { return descriptorSetLayout_; }

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
	};
}