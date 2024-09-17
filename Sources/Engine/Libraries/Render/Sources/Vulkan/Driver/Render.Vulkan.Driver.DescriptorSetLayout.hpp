#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Common.Types.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class DescriptorSetLayout : public Abstraction<VkDescriptorSetLayout>{
	public:

		struct CreateInfo {
			std::string name_ = "No name";
			std::shared_ptr<LogicDevice> logicDevice_;
			std::vector<VkDescriptorSetLayoutBinding> bindings_;
		};

		DescriptorSetLayout(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			{
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = static_cast<Common::UInt32>(createInfo.bindings_.size());
				layoutInfo.pBindings = createInfo.bindings_.data();
			}
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
			VkCall(vkCreateDescriptorSetLayout(createInfo.logicDevice_->GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout),
				"Error while creating descriptor set layout.");
			SetHandle(descriptorSetLayout);
		}
		~DescriptorSetLayout() {
			vkDestroyDescriptorSetLayout(createInfo_.logicDevice_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		CreateInfo createInfo_;
	};

	using DSL = DescriptorSetLayout;

}