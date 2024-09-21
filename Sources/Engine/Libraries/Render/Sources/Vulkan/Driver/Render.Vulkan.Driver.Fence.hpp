#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class Fence final {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_;
			VkFenceCreateFlags flags_ = VK_FENCE_CREATE_SIGNALED_BIT;
		};

		Fence(const CreateInfo& createInfo) : LD_{ createInfo.LD_ } {
			VkFenceCreateInfo fenceInfo{};
			{
				fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.flags = createInfo.flags_;
			}
			VkFence fence = VK_NULL_HANDLE;
			[[maybe_unused]]
			const VkResult result = vkCreateFence(createInfo.LD_->GetHandle(), &fenceInfo, nullptr, &fence);
			OS::AssertMessage(result == VK_SUCCESS, "Error while creating fence.");
			SetNative(fence);
		}


		Fence(Fence&& moveFence) noexcept :  LD_{ nullptr }, fence_{ VK_NULL_HANDLE } {
			std::swap(fence_, moveFence.fence_);
			std::swap(LD_, moveFence.LD_);
		}

		Fence& operator=(Fence&& moveFence) noexcept {

			if (this == &moveFence) {
				return *this;
			}

			Destroy();

			std::swap(fence_, moveFence.fence_);
			std::swap(LD_, moveFence.LD_);

			return *this;

		}

		Fence() noexcept = default;

		Fence(VkFence fence) noexcept : fence_{ fence } {}

		void Wait() noexcept {
			[[maybe_unused]]
			const VkResult result = vkWaitForFences(LD_->GetHandle(), 1, &GetNative(), VK_TRUE, UINT64_MAX);
			OS::AssertMessage(result == VK_SUCCESS, "Errow while waitting fence.");
		}

		void Reset() noexcept {
			[[maybe_unused]]
			const VkResult result = vkResetFences(LD_->GetHandle(), 1, &GetNative());
			OS::AssertMessage(result == VK_SUCCESS, "Error while resetting fence.");
		}

		[[nodiscard]]
		bool IsSignaled() const noexcept {
			const VkResult result = vkGetFenceStatus(LD_->GetHandle(), GetNative());
			return result == VK_SUCCESS;
		}

		[[nodiscard]]
		const VkFence& GetNative() const noexcept { return fence_; }

		~Fence() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(GetNative() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkFence.");
			OS::Assert(LD_ != nullptr);
			vkDestroyFence(LD_->GetHandle(), fence_, nullptr);
			SetNative(VK_NULL_HANDLE);
		}

		void SetNative(VkFence fence) noexcept {
			OS::Assert(
				(fence != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((fence == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			fence_ = fence;
		}

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
		VkFence fence_ = VK_NULL_HANDLE;
	};

}