#pragma once

#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Abstraction;

import OksEngine.OS;

namespace Render::Vulkan {

	template<class VkAbstractionType>
	class [[nodiscard]] Abstraction {
	public:

		Abstraction() noexcept = default;
		explicit Abstraction(VkAbstractionType handle) noexcept : handle_ { handle } { }

		[[nodiscard]]
		VkAbstractionType GetHandle() const noexcept { return handle_; }

		void SetHandle(VkAbstractionType handle) noexcept {

			OS::Assert((handle != VK_NULL_HANDLE) && (GetHandle() == VK_NULL_HANDLE) ||
				((handle == VK_NULL_HANDLE) && (GetHandle() != VK_NULL_HANDLE)));
			handle_ = handle;
		}

		[[nodiscard]]
		inline operator VkAbstractionType() const noexcept {
			return GetHandle();
		}

	private:
		VkAbstractionType handle_ = VK_NULL_HANDLE;
	};

}