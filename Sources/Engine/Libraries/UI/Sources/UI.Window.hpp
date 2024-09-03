#pragma once 

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <memory>
#include <any>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <queue>

#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <UI.hpp>

#include <UI/OksEngine.UIAL.Window.hpp>

namespace UI {


	class Window: public UIAL::Window  {
	public:

		Window(const CreateInfo& createInfo) : UIAL::Window{ createInfo } {
			const int initResult = glfwInit();
			OS::AssertMessage(initResult != GLFW_FALSE, "Error while initializing GLFW.");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			GLFWwindow* createdWindow = glfwCreateWindow(
				createInfo.windowSize_.GetX(),
				createInfo.windowSize_.GetY(),
				"Vulkan",
				nullptr,
				nullptr);
			glfwSetInputMode(createdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(createdWindow, [](GLFWwindow* window, double xpos, double ypos) {

				static double xPrevious = xpos;
				static double yPrevious = ypos;
				const double deltaX = xPrevious - xpos;
				const double deltaY = yPrevious - ypos;
				if (!Math::IsEqual(deltaX, 0.0) || !Math::IsEqual(deltaY, 0.0)) {
					xPrevious = xpos;
					yPrevious = ypos;
					Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
					MouseEvent event;
					event.position_ = { xpos, ypos };
					event.offset_ = { deltaX, deltaY };
					windowPtr->PushEvent(event);
				}

				});

			ImGui::CreateContext();
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			ImPlot::CreateContext();
			ImGui_ImplGlfw_InitForVulkan(createdWindow, true);

			OS::AssertMessage(
				createdWindow != nullptr,
				"GLFW Windows was not created.");
			glfwSetWindowUserPointer(createdWindow, this);
			glfwSetKeyCallback(createdWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
				KeyboardKey keyboardKey = KeyboardKey::Undefined;
				Event event = Event::Undefined;
				keyboardKey = static_cast<KeyboardKey>(key);
				if (action == GLFW_PRESS) {
					event = Event::Pressed;
				}
				else if (action == GLFW_RELEASE) {
					event = Event::Released;
				}
				else {
					return;
				}
				windowPtr->PushEvent(KeyboardEvent{ keyboardKey, event });

				});

			window_ = createdWindow;
		}

		virtual void DisableCursor() override {
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		virtual void EnableCursor() override {
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		
		[[nodiscard]]
		Info GetInfo(UIAL::Render render) const noexcept {
			Info info;
			if (render == UIAL::Render::Vulkan) {
				uint32_t count;
				const char** extensions = glfwGetRequiredInstanceExtensions(&count);
				info.param1_ = window_;
				info.param2_ = count;
				info.param3_ = extensions;
				info.size_ = GetSize();
				info.subsystem_ = UIAL::Subsystem::GLFW;
			}
			return info;
		}

		[[nodiscard]]
		virtual glm::u32vec2 GetSize() const noexcept override {
			int width = 0;
			int height = 0;
			glfwGetWindowSize(window_, &width, &height);
			OS::Assert(width > 0 && height > 0);
			return { static_cast<Common::UInt32>(width), static_cast<Common::UInt32>(height) };
		}

		virtual void SetTitle(const std::string& title) noexcept override {
			glfwSetWindowTitle(window_, title.c_str());
		}

		virtual void Show() override {

		}

		virtual void ProcessInput() override {
			ImGui_ImplGlfw_NewFrame();
			if (!glfwWindowShouldClose(window_)) {
				glfwPollEvents();
			}
		}

		~Window() {
			glfwDestroyWindow(window_);
			glfwTerminate();
			OS::LogInfo("/Window/", "GLFW Window  destroyed successfuly.");
		}

	private:
		static void ErrorCallback(int error_code, const char* description) {
			OS::AssertFailMessage(description);
		}

	private:
		GLFWwindow* window_ = nullptr;

	};

	class API /*: public UIAL::API*/ {
	public:
#undef CreateWindow
		std::shared_ptr<Window> CreateWindow(const Window::CreateInfo& createInfo) const {
			return std::make_shared<Window>(createInfo);
		}
	};


}