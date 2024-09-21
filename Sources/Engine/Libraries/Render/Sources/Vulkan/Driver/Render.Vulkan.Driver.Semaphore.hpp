#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class Semaphore {
	public:
		Semaphore() = delete;

		Semaphore(Semaphore&& moveSemaphore) : LD_{ nullptr }, semaphore_{ VK_NULL_HANDLE }  {
			std::swap(semaphore_, moveSemaphore.semaphore_);
			std::swap(LD_, moveSemaphore.LD_);
		}

		Semaphore(std::shared_ptr<LogicDevice> logicDevice) : LD_{ logicDevice } {
			VkSemaphoreCreateInfo semaphoreInfo{};
			{
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			}
			VkSemaphore semaphore = VK_NULL_HANDLE;
			[[maybe_unused]]
			const VkResult result = vkCreateSemaphore(logicDevice->GetHandle(), &semaphoreInfo, nullptr, &semaphore);
			OS::AssertMessage(result == VK_SUCCESS, "Error while creating semaphore.");
			SetNative(semaphore);
		}

		void Destroy() noexcept {
			OS::AssertMessage(semaphore_ != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkSemaphore.");
			OS::AssertMessage(LD_ != nullptr, "Logic device is not initialized.");
			vkDestroySemaphore(LD_->GetHandle(), semaphore_, nullptr);
			SetNative(VK_NULL_HANDLE);
		}

		~Semaphore() noexcept {
			OS::Assert(
				((GetNative() != VK_NULL_HANDLE) && (LD_ != nullptr)) ||
				((GetNative() == VK_NULL_HANDLE) && (LD_ == nullptr)));
			if (GetNative() != VK_NULL_HANDLE) {
				Destroy();
			}
		}

		[[nodiscard]]
		const VkSemaphore& GetNative() const noexcept { return semaphore_; }

	private:

		void SetNative(VkSemaphore semaphore) noexcept {
			OS::Assert(
				(semaphore != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((semaphore == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			semaphore_ = semaphore;
		}

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
		VkSemaphore semaphore_ = VK_NULL_HANDLE;
	};


}