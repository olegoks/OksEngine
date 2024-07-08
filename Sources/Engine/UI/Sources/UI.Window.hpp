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


#include <Math.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <UI.hpp>

namespace UI {


	class Window/* : public UIAL::Window */ {
	public:

		using Size = Math::Vector2i;

		struct CreateInfo {
			Size windowSize_;
			std::string title_;
		};

		enum class Key : Common::UInt64 {
			Q,
			W,
			E,
			A,
			S,
			D,
			Space,
			F5,
			Undefined
		};
		enum class Event : Common::UInt64 {
			Pressed,
			Released,
			Undefined
		};

		using KeyboardEventCallback = std::function<void(Key, Event)>;
		using MouseEventCallback = std::function<void(const Math::Vector2d& position)>;

		Window(const CreateInfo& createInfo) : createInfo_{ createInfo } {
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
					windowPtr->CallMouseCallbacks({ deltaX, deltaY });
				}

				});

			ImGui::CreateContext();
			ImPlot::CreateContext();
			ImGui_ImplGlfw_InitForVulkan(createdWindow, true);

			OS::AssertMessage(createdWindow != nullptr, "GLFW Windows was not created.");
			glfwSetWindowUserPointer(createdWindow, this);
			glfwSetKeyCallback(createdWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				Key keyboardKey;
				Event event;

				if (key == GLFW_KEY_W) {
					keyboardKey = Key::W;
				} else if (key == GLFW_KEY_S) {
					keyboardKey = Key::S;
				} else if (key == GLFW_KEY_A) {
					keyboardKey = Key::A;
				} else if (key == GLFW_KEY_D) {
					keyboardKey = Key::D;
				}
				else if (key == GLFW_KEY_Q) {
					keyboardKey = Key::Q;
				}
				else if (key == GLFW_KEY_SPACE) {
					keyboardKey = Key::Space;

				}
				else if (key == GLFW_KEY_F5) {
					keyboardKey = Key::F5;

				}
				else if (key == GLFW_KEY_E) {
					keyboardKey = Key::E;

				} else {
					return;
				}

				if (action == GLFW_PRESS) {
					event = Event::Pressed;
				}
				else if (action == GLFW_RELEASE) {
					event = Event::Released;
				}
				else {
					return;
				}

				Window* windowPtr = (Window*)glfwGetWindowUserPointer(window);
				windowPtr->CallEventCallbacks(keyboardKey, event);

				});

			window_ = createdWindow;
		}

		struct Info {
			std::any param1_;
			std::any param2_;
			std::any param3_;
			std::any param4_;
			Math::Vector2u32 size_{ 0, 0 };
			Subsystem subsystem_ = Subsystem::Undefined;
		};

		[[nodiscard]]
		Info GetInfo(Render render) const noexcept {
			Info info;
			if (render == Render::Vulkan) {
				uint32_t count;
				const char** extensions = glfwGetRequiredInstanceExtensions(&count);
				info.param1_ = window_;
				info.param2_ = count;
				info.param3_ = extensions;
				info.size_ = GetSize();
				info.subsystem_ = Subsystem::GLFW;
			}
			return info;
		}

		[[nodiscard]]
		Math::Vector2u32 GetSize() const noexcept {
			int width = 0;
			int height = 0;
			glfwGetWindowSize(window_, &width, &height);
			OS::Assert(width > 0 && height > 0);
			return { static_cast<Common::UInt32>(width), static_cast<Common::UInt32>(height) };
		}

		void SetTitle(const std::string& title) noexcept {
			glfwSetWindowTitle(window_, title.c_str());
		}

		void Show() {

		}

		void ProcessInput() {
			if (!glfwWindowShouldClose(window_)) {
				glfwPollEvents();

			}
		}

		~Window() {
			glfwDestroyWindow(window_);

			glfwTerminate();
			OS::LogInfo("/Window/", "GLFW Window  destroyed successfuly.");
		}

		void RegisterKeyboardEventCallback(KeyboardEventCallback&& eventCallback) {
			keyboardEventCallbacks_.push_back(std::move(eventCallback));
		}

		void RegisterMouseEventCallback(MouseEventCallback&& eventCallback) {
			mouseEventCallbacks_.push_back(std::move(eventCallback));
		}

	private:
		static void ErrorCallback(int error_code, const char* description) {
			OS::AssertFailMessage(description);
		}
	private:
		void CallEventCallbacks(Key key, Event event) const noexcept {
			for (const KeyboardEventCallback& callback : keyboardEventCallbacks_) {
				callback(key, event);
			}
		}
		void CallMouseCallbacks(const Math::Vector2d& position) const noexcept {
			for (const MouseEventCallback& callback : mouseEventCallbacks_) {
				callback(position);
			}
		}
		std::vector<KeyboardEventCallback> keyboardEventCallbacks_;
		std::vector<MouseEventCallback> mouseEventCallbacks_;
	private:
		CreateInfo createInfo_;
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