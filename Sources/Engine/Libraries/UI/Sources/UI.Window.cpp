#include <UI.Window.hpp>


#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui_impl_glfw.h>


namespace UI {

	UIAL::Window::CursorMode Window::ConvertCursorMode(int glfwMode) {

		if (glfwMode == GLFW_CURSOR_NORMAL) {
			return UIAL::Window::CursorMode::Normal;
		}
		else if (glfwMode == GLFW_CURSOR_HIDDEN) {
			return UIAL::Window::CursorMode::Hidden;
		}
		else if (glfwMode == GLFW_CURSOR_CAPTURED) {
			return UIAL::Window::CursorMode::Captured;
		}
		else if (glfwMode == GLFW_CURSOR_DISABLED) {
			return UIAL::Window::CursorMode::Disabled;
		}
		else {
			ASSERT_FAIL_MSG("Invalid glfw mode.");
			return UIAL::Window::CursorMode::Undefined;
		}

	}

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

		// урсор скрыт
		//  урсор захвачен окном(не может покинуть его)
		// ѕри движении мыши курсор остаетс€ в центре окна
		//  оординаты мыши сообщаютс€ как относительные смещени€(deltas)
		glfwSetInputMode(createdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);/*GLFW_CURSOR_HIDDEN);*///GLFW_CURSOR_NORMAL);
		freeCursor_ = false;

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

				int mode = glfwGetInputMode(window, GLFW_CURSOR);

				static double xPrevious = xpos;
				static double yPrevious = ypos;
				const double deltaX = xPrevious - xpos;
				const double deltaY = yPrevious - ypos;
				if (!Math::IsEqual(deltaX, 0.0) || !Math::IsEqual(deltaY, 0.0)) {
					xPrevious = xpos;
					yPrevious = ypos;
					Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
					CursorEvent event;
					event.mode_ = ConvertCursorMode(mode);
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
		io.DisplaySize = ImGui::GetMainViewport()->Size;
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

		glfwSetWindowSizeCallback(createdWindow, [](::GLFWwindow* window, int width, int height) {

			Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);

			UIAL::Window::WorkAreaResizeEvent event{
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
		int value = glfwGetInputMode(reinterpret_cast<::GLFWwindow*>(window_), GLFW_CURSOR);
		ASSERT(value == GLFW_CURSOR_DISABLED);

	}

	void Window::EnableCursor() {
		
		glfwSetInputMode(reinterpret_cast<::GLFWwindow*>(window_), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		int value = glfwGetInputMode(reinterpret_cast<::GLFWwindow*>(window_), GLFW_CURSOR);
		ASSERT(value == GLFW_CURSOR_NORMAL);
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
			info.size_ = GetWorkAreaSize();
			info.subsystem_ = UIAL::Subsystem::GLFW;
		}
		return info;
	}

	[[nodiscard]]
	glm::u32vec2 Window::GetFramebufferSize() const noexcept {

		int width = -1;
		int height = -1;

		glfwGetFramebufferSize(
			reinterpret_cast<::GLFWwindow*>(window_),
			&width, &height);

		return { width, height };

	}

	[[nodiscard]]
	glm::u32vec2 Window::GetWorkAreaSize() const noexcept {
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