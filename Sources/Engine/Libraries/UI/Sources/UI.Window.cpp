#include <UI.Window.hpp>


#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui_impl_glfw.h>


namespace UI {

	Window::Window(const Window::CreateInfo& createInfo) : UIAL::Window{ createInfo } {
		const int initResult = glfwInit();
		ASSERT_FMSG(initResult != GLFW_FALSE, "Error while initializing GLFW.");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		::GLFWwindow* createdWindow = glfwCreateWindow(
			createInfo.windowSize_.GetX(),
			createInfo.windowSize_.GetY(),
			"OksEngineGame",
			nullptr,
			nullptr);

		glfwMakeContextCurrent(createdWindow);

		glfwSetInputMode(createdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);/*GLFW_CURSOR_HIDDEN);*///GLFW_CURSOR_NORMAL);


		//Keyboard callback.
		glfwSetKeyCallback(createdWindow, [](::GLFWwindow* window, int key, int scancode, int action, int mods) {

			//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

			Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
			KeyboardKey keyboardKey = KeyboardKey::Undefined;
			KeyAction event = KeyAction::Undefined;
			keyboardKey = static_cast<KeyboardKey>(key);
			if (action == GLFW_PRESS) {
				event = KeyAction::Pressed;
			}
			else if (action == GLFW_RELEASE) {
				event = KeyAction::Released;
			}
			else {
				return;
			}
			windowPtr->PushEvent(KeyboardEvent{ keyboardKey, event });

			});

		//Cursor callback.
		glfwSetCursorPosCallback(createdWindow,
			[](::GLFWwindow* window, double xpos, double ypos) {

				//OS::LogInfo("cursor_pos", { "X: {}, Y: {}", xpos, ypos });

				static double xPrevious = xpos;
				static double yPrevious = ypos;
				const double deltaX = xPrevious - xpos;
				const double deltaY = yPrevious - ypos;
				if (!Math::IsEqual(deltaX, 0.0) || !Math::IsEqual(deltaY, 0.0)) {
					xPrevious = xpos;
					yPrevious = ypos;
					Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
					CursorEvent event;
					event.position_ = { xpos, ypos };
					event.offset_ = { deltaX, deltaY };
					windowPtr->PushEvent(event);
				}

			});


		glfwSetMouseButtonCallback(createdWindow,
			[](::GLFWwindow* window, int button, int action, int mods) {

				MouseEvent event;
				if (action == GLFW_PRESS) {
					event.event_ = KeyAction::Pressed;
				}
				else if (action == GLFW_RELEASE) {
					event.event_ = KeyAction::Released;
				}
				else {
					return;
				}

				
				MouseKey key = MouseKey::Undefined;
				
				if (button == GLFW_MOUSE_BUTTON_LEFT) {
					event.key_ = MouseKey::Left;
				}
				else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
					event.key_ = MouseKey::Right;
				}
				else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
					event.key_ = MouseKey::Middle;
				}
				else {
					return;
				}

				Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);

				windowPtr->PushEvent(event);

			});

		glfwSetScrollCallback(createdWindow, [](::GLFWwindow* window, double xoffset, double yoffset) {

			MouseEvent event;

			event.scroll_ += yoffset;

			Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);

			windowPtr->PushEvent(event);
			});

		ImGui::CreateContext();
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.WantCaptureMouse = true;
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(createdWindow, true);

		if (!glfwVulkanSupported()) {
			OS::AssertFail();
		}

		ASSERT_MSG(
			createdWindow != nullptr,
			"GLFW Windows was not created.");

		glfwSetWindowUserPointer(createdWindow, this);

		glfwSetFramebufferSizeCallback(createdWindow, [](::GLFWwindow* window, int width, int height) {

			Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);

			UIAL::Window::FrameBufferResizeEvent event{
				width,
				height
			};

			windowPtr->PushEvent(event);

			});

		glfwSetErrorCallback([](int error, const char* description) {
			std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
			});


		window_ = reinterpret_cast<GLFWwindow*>(createdWindow);
	}


	void Window::DisableCursor() {
		//glfwSetWindowAttrib(reinterpret_cast<::GLFWwindow*>(window_), GLFW_FOCUSED, GLFW_TRUE);
		glfwSetInputMode(reinterpret_cast<::GLFWwindow*>(window_), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}

	void Window::EnableCursor() {

		glfwSetInputMode(reinterpret_cast<::GLFWwindow*>(window_), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);


	}

	[[nodiscard]]
	Window::Info Window::GetInfo(UIAL::Render render) const noexcept {
		Info info;
		if (render == UIAL::Render::Vulkan) {
			uint32_t count;
			const char** extensions = glfwGetRequiredInstanceExtensions(&count);
			info.param1_ = reinterpret_cast<::GLFWwindow*>(window_);
			info.param2_ = count;
			info.param3_ = extensions;
			info.size_ = GetSize();
			info.subsystem_ = UIAL::Subsystem::GLFW;
		}
		return info;
	}

	[[nodiscard]]
	glm::u32vec2 Window::GetSize() const noexcept {
		int width = 0;
		int height = 0;
		glfwGetWindowSize(reinterpret_cast<::GLFWwindow*>(window_), &width, &height);
		OS::Assert(width > 0 && height > 0);
		return { static_cast<Common::UInt32>(width), static_cast<Common::UInt32>(height) };
	}

	void Window::SetTitle(const std::string& title) noexcept {
		glfwSetWindowTitle(reinterpret_cast<::GLFWwindow*>(window_), title.c_str());
	}

	void Window::Show() {

	}

	void Window::ProcessInput() {
		ImGui_ImplGlfw_NewFrame();
		if (!glfwWindowShouldClose(reinterpret_cast<::GLFWwindow*>(window_))) {

			glfwPollEvents();
		}
	}

	Window::~Window() {
		glfwDestroyWindow(reinterpret_cast<::GLFWwindow*>(window_));
		glfwTerminate();
		OS::LogInfo("/Window/", "GLFW Window  destroyed successfuly.");
	}


}