#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>


#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>

namespace Render::Vulkan {

	class CommandPool : public Abstraction<VkCommandPool>{
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_;
			QueueFamily queueFamily_;
		};

		CommandPool(const CreateInfo& createInfo) noexcept :
			createInfo_{ createInfo } {

			VkCommandPoolCreateInfo poolInfo{};
			{
				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.queueFamilyIndex = createInfo.queueFamily_.index_;
				poolInfo.flags = 0; // Optional
			}

			VkCommandPool commandPool = VK_NULL_HANDLE;
			VK_CALL(vkCreateCommandPool(createInfo.LD_->GetHandle(), &poolInfo, nullptr, &commandPool),
				"Error while creating command pool." );
			SetHandle(commandPool);
			OS::LogInfo("/render/vulkan/driver", "Command pool was created successfuly.");
		}

		~CommandPool() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			ASSERT_FMSG(createInfo_.LD_ != nullptr, "Logic device is nullptr.");
			ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkCommandPool.");
			vkDestroyCommandPool(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
			SetHandle(VK_NULL_HANDLE);
		}

	private:
		CreateInfo createInfo_;
	};


}