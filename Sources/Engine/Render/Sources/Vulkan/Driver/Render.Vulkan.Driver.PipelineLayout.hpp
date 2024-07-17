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
			std::vector<VkPushConstantRange> pushConstants_;
			std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts_;
		};

		PipelineLayout(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			{
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = static_cast<Common::UInt32>(createInfo.descriptorSetLayouts_.size()); // Optional
				std::vector<VkDescriptorSetLayout> descriptorSetLayoutsHandles;
				for (auto descriptorSetLayout : createInfo.descriptorSetLayouts_) {
					descriptorSetLayoutsHandles.push_back(*descriptorSetLayout);
				}
				pipelineLayoutInfo.pSetLayouts = descriptorSetLayoutsHandles.data(); // Optional
				pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)createInfo.pushConstants_.size(); // Optional
				pipelineLayoutInfo.pPushConstantRanges = createInfo.pushConstants_.data(); // Optional


				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkCall(vkCreatePipelineLayout(createInfo.logicDevice_->GetHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
					"Error while creating pipeline layout.");
				SetHandle(pipelineLayout);
			}

		}

		[[nodiscard]]
		const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDSLs() const noexcept {
			return createInfo_.descriptorSetLayouts_;
		}

		~PipelineLayout() {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkPipelineLayout.");
			vkDestroyPipelineLayout(*createInfo_.logicDevice_, GetHandle(), nullptr);
			SetHandle(VK_NULL_HANDLE);
		}


	private:
		CreateInfo createInfo_;
	};

}