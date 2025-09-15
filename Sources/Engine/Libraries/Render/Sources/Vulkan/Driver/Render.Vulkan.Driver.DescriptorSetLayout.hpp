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

		struct CreateInfo final {
			static inline const char* const defaultDSLName_ = "No Name";
			std::string name_ = defaultDSLName_;
			std::shared_ptr<LogicDevice> LD_;
			std::vector<VkDescriptorSetLayoutBinding> bindings_;
		};

		explicit DescriptorSetLayout(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {

			ASSERT_FMSG(createInfo.name_ != CreateInfo::defaultDSLName_, "Please, set name to descriptor set layout create info.");
			ASSERT_FMSG(createInfo.LD_ != nullptr, "Please, set Physical Device to descriptor set layout create info.");

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			{
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = static_cast<Common::UInt32>(createInfo.bindings_.size());
				layoutInfo.pBindings = (!createInfo.bindings_.empty()) ? (createInfo.bindings_.data()) : (nullptr);
			}
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
			VkCall(vkCreateDescriptorSetLayout(createInfo.LD_->GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout),
				"Error while creating descriptor set layout.");
			SetHandle(descriptorSetLayout);
		}
		~DescriptorSetLayout() {
			ASSERT_FMSG(!IsNullHandle(), "Attempt to delete null DSL.");
			vkDestroyDescriptorSetLayout(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
		}

		[[nodiscard]]
		const std::vector<VkDescriptorSetLayoutBinding> GetBindings() const noexcept {
			return createInfo_.bindings_;
		}

	private:
		CreateInfo createInfo_;
	};

	using DSL = DescriptorSetLayout;

}