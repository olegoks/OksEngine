#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

namespace Render::Vulkan {

	class ImGUI {
	public:
		struct CreateInfo {
			GLFWwindow* window_ = nullptr;

		};
		ImGUI(const CreateInfo& createInfo) {
			//ImGui::CreateContext();
			////ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;//ImGuiConfigFlags_DockingEnable;
			//ImGui_ImplGlfw_InitForVulkan(createInfo.window_, true);
			//ImGui_ImplVulkan_InitInfo init_info = {};
			//init_info.Instance = *instance_;
			//init_info.PhysicalDevice = *physicalDevice_;
			//init_info.Device = *logicDevice_;
			//init_info.QueueFamily = *graphicsQueueFamily_;
			//init_info.Queue = queue;
			//init_info.PipelineCache = nullptr;
			//init_info.DescriptorPool = *descriptorPool_;
			//init_info.Allocator = nullptr;
			//init_info.MinImageCount = 3;
			//init_info.ImageCount = 3;
			//init_info.CheckVkResultFn = nullptr;
			//ImGui_ImplVulkan_Init(&init_info, renderPass_);
		}
	private:
	};

}