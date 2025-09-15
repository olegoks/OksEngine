#include <Render.Vulkan.Driver.WindowSurface.hpp>
#include <Render.Vulkan.Driver.Instance.hpp>
#include <GLFW/glfw3.h>

//#include <RAL.Driver.hpp>

namespace Render::Vulkan {
	WindowSurface::WindowSurface(const CreateInfo& createInfo) noexcept :
		instance_{ createInfo.instance_ } {
		ASSERT_FMSG(createInfo.instance_ != nullptr,
			"Attempt to create VkSurfaceKHR with nullprtr VkInstance.");
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		RAL::RenderSurface renderSurface = createInfo.renderSurface_;
		if (renderSurface.uiSubsystem_ == RAL::UISubsystem::GLFW) {
			auto* window = std::any_cast<GLFWwindow*>(createInfo.renderSurface_.param1_);
			VkCall(glfwCreateWindowSurface(*createInfo.instance_, window, nullptr, &surface),
				"Error while creating window surface.");
			SetHandle(surface);
		} else {
			ASSERT_FAIL_MSG("Attempt to create VkWindowSurface with unsupported ui subsystem.");
		}
	}

	void WindowSurface::Destroy() noexcept {
		ASSERT_FMSG(instance_ != nullptr, "Instance pointer was not initialized.");
		ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE on destroying.");
		vkDestroySurfaceKHR(instance_->GetHandle(), GetHandle(), nullptr);
	}
}
