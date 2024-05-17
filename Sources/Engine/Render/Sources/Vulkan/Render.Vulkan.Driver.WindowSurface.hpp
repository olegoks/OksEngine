#pragma once

#include <memory>
#include <windows.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_win32.h>
#include <any>

export module OksEngine.Render.Vulkan.Driver.WindowSurface;

import OksEngine.Render.Vulkan.Abstraction;
import OksEngine.Render.Vulkan.Driver.Instance;
import OksEngine.OS.Assert;
import OksEngine.OS.Logger;
import OksEngine.UIAL;

namespace Render::Vulkan {

	class [[nodiscard]] WindowSurface final : public Abstraction<VkSurfaceKHR> {
	public:

		struct CreateInfo {
			std::shared_ptr<Instance> instance_ = nullptr;
			UIAL::Window::Info windowInfo_;
		};

		WindowSurface(const CreateInfo& createInfo) noexcept : instance_ { createInfo.instance_ } {

			OS::AssertMessage(createInfo.instance_ != nullptr, "Attempt to create VkSurfaceKHR with nullprtr VkInstance.");
			OS::AssertMessage(createInfo.windowInfo_.info_.has_value(), "Attempt to create VkSurfaceKHR without UI native API info.");

			if ((createInfo.windowInfo_.nativeAPI_ == UIAL::Window::NativeAPI::GLFW)
				|| (createInfo.windowInfo_.nativeAPI_ == UIAL::Window::NativeAPI::WinAPI)) {
				const auto winAPIWindowInfo = std::any_cast<UIRequiredInfo>(createInfo.windowInfo_.info_);
				VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
				{
					surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
					surfaceCreateInfo.hwnd = winAPIWindowInfo.windowHandle_;
					surfaceCreateInfo.hinstance = winAPIWindowInfo.instanceHandle_;
				}
				VkSurfaceKHR surface = VK_NULL_HANDLE;
				VkCall(vkCreateWin32SurfaceKHR(createInfo.instance_->GetNative(), &surfaceCreateInfo, nullptr, &surface), "Error while creating window surface.");
				SetHandle(surface);
			} else {
				OS::AssertFailMessage("Used native UI API that RAL doesn't support.");
			}
			
		}

		~WindowSurface() noexcept {

			Destroy();
			
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(instance_ != nullptr, "Instance pointer was not initialized.");
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE on destroying.");
			vkDestroySurfaceKHR(instance_->GetNative(), GetHandle(), nullptr);
		}

	private:

		std::shared_ptr<Instance> instance_ = nullptr;

	};

}