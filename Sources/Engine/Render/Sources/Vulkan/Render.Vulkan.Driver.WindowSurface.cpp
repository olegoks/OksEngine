

#include <Render.Vulkan.Driver.WindowSurface.hpp>

#include <GLFW/glfw3.h>

#include <RAL.Driver.hpp>

namespace Render::Vulkan {
	WindowSurface::WindowSurface(const CreateInfo& createInfo) noexcept :
		instance_{ createInfo.instance_ } {
		OS::AssertMessage(createInfo.instance_ != nullptr,
			"Attempt to create VkSurfaceKHR with nullprtr VkInstance.");
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		RAL::RenderSurface renderSurface = createInfo.renderSurface_;
		if (renderSurface.uiSubsystem_ == RAL::UISubsystem::GLFW) {
			auto window = std::any_cast<GLFWwindow*>(createInfo.renderSurface_.param2_);
			VkCall(glfwCreateWindowSurface(*createInfo.instance_.get(), window, nullptr, &surface),
				"Error while creating window surface.");
			SetHandle(surface);
		}
	}
	void WindowSurface::Destroy() noexcept {
		OS::AssertMessage(instance_ != nullptr, "Instance pointer was not initialized.");
		OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE on destroying.");
		vkDestroySurfaceKHR(instance_->GetNative(), GetHandle(), nullptr);
	}
}
