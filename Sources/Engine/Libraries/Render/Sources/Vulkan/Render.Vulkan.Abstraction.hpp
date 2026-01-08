#pragma once

#include <Render.Vulkan.Common.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

namespace Render::Vulkan {

	template<class VkAbstractionType>
	class [[nodiscard]] Abstraction {
	public:

		Abstraction() noexcept = default;
		explicit Abstraction(VkAbstractionType handle) noexcept : handle_ { handle } { }

		

		[[nodiscard]]
		const VkAbstractionType& GetHandle() const noexcept { 
			/*ASSERT(IsHandleValid());*/
			return handle_;
		}

		[[nodiscard]]
		bool IsNullHandle() const noexcept {
			return (GetHandle() == VK_NULL_HANDLE);
		}

		[[nodiscard]]
		bool IsHandleValid() const noexcept {
			return !IsNullHandle();
		}

		void SetHandle(VkAbstractionType handle) noexcept {

			OS::Assert((handle != VK_NULL_HANDLE) && (GetHandle() == VK_NULL_HANDLE) ||
				((handle == VK_NULL_HANDLE) && (GetHandle() != VK_NULL_HANDLE)));
			handle_ = handle;
		}

		void SetNullHandle() noexcept {
			SetHandle(VK_NULL_HANDLE);
		}

		[[nodiscard]]
		inline operator VkAbstractionType() const noexcept {
			return GetHandle();
		}

	private:
		VkAbstractionType handle_ = VK_NULL_HANDLE;
	};

}