#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>


#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>

namespace Render::Vulkan {

	class CommandPool {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			QueueFamily queueFamily_;
		};

		CommandPool(const CreateInfo& createInfo) noexcept :
			logicDevice_{ createInfo.logicDevice_ } {

			VkCommandPoolCreateInfo poolInfo{};
			{
				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.queueFamilyIndex = createInfo.queueFamily_.index_;
				poolInfo.flags = 0; // Optional
			}

			VkCall(vkCreateCommandPool(createInfo.logicDevice_->GetHandle(), &poolInfo, nullptr, &commandPool_),
				"Error while creating command pool." );
			OS::LogInfo("/render/vulkan/driver", "Command pool was created successfuly.");
		}
		~CommandPool() noexcept {
			Destroy();
		}

		[[nodiscard]]
		const VkCommandPool& GetNative() const noexcept { return commandPool_; }

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is nullptr.");
			OS::AssertMessage(GetNative() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkCommandPool.");
			vkDestroyCommandPool(logicDevice_->GetHandle(), GetNative(), nullptr);
			SetNative(VK_NULL_HANDLE);
		}

		void SetNative(VkCommandPool commandPool) noexcept {
			OS::Assert((commandPool != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((commandPool == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			commandPool_ = commandPool;
		}

	private:

		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		VkCommandPool commandPool_;
	};


}