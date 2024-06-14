#pragma once

#include <memory>
#include <vulkan\vulkan.hpp>

#include <OS.Assert.hpp>

#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>

namespace Render::Vulkan {

	class PipelineLayout : public Abstraction<VkPipelineLayout> {
	public:
		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr< DescriptorSetLayout> descriptorSetLayout_;
		};

		PipelineLayout(const CreateInfo& createInfo) noexcept : logicDevice_{ createInfo.logicDevice_ } {

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			{
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 1; // Optional
				pipelineLayoutInfo.pSetLayouts = &createInfo.descriptorSetLayout_->GetNative(); // Optional
				pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
				pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional


				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				[[maybe_unused]]
				const VkResult result = vkCreatePipelineLayout(createInfo.logicDevice_->GetHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
				OS::AssertMessage(result == VK_SUCCESS, "Error while creating pipeline layout.");
				SetHandle(pipelineLayout);
			}

		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is not initialized.");
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkPipelineLayout.");
			vkDestroyPipelineLayout(logicDevice_->GetHandle(), GetHandle(), nullptr);
			SetHandle(VK_NULL_HANDLE);
		}


	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
	};

}