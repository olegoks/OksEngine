#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <memory>
#include <any>
#include <vector>
#include <string>
#include <functional>


#include <Math.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

//import OksEngine.RAL;
//import OksEngine.Render.Vulkan.Common;
//
//import OksEngine.UIAL;
//import OksEngine.Common.Types;
//import OksEngine.Math.Vector;
//import OksEngine.OS.Logger;
//import OksEngine.OS.Assert;


namespace UI {

	class Window/* : public UIAL::Window */{
	public:

		using Size = Math::Vector2i;

		struct CreateInfo {
			Size windowSize_;
			std::string title_;
		};

		enum class Key : Common::UInt64 {
			W,
			A,
			S,
			D
		};
		enum class Event : Common::UInt64 {
			Pressed
		};


		using EventCallback = std::function<void(Key, Event)>;

		Window(const CreateInfo& createInfo) : createInfo_{ createInfo } {
			const int initResult = glfwInit();
			OS::AssertMessage(initResult != GLFW_FALSE, "Error while initializing GLFW.");
			GLFWwindow* createdWindow =  glfwCreateWindow(
				createInfo.windowSize_.GetX(),
				createInfo.windowSize_.GetY(),
				"Vulkan",
				nullptr,
				nullptr);
			OS::AssertMessage(createdWindow != nullptr, "GLFW Windows was not created.");
			glfwSetWindowUserPointer(createdWindow, this);
			glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				Key keyboardKey;
				Event event;

				if (key == GLFW_KEY_W && action == GLFW_PRESS) {
					keyboardKey = Key::W;
					event = Event::Pressed;
				}
				else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
					keyboardKey = Key::S;
					event = Event::Pressed;
				}
				else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
					keyboardKey = Key::A;
					event = Event::Pressed;
				}
				else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
					keyboardKey = Key::D;
					event = Event::Pressed;
				}

				//Window* thisWindow = (Window*)glfwGetWindowUserPointer(window);
				//for (auto& callback : thisWindow->eventCallbacks_) {
				//	callback(keyboardKey, event);
				//}
				});
		}

		//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		//{

		//}

		Window(/*const UIAL::Window::Size& size*/) /*: size_{ size } */{
			/*glfwSetErrorCallback(&ErrorCallback);
			[[maybe_unused]]
			const int result = glfwInit();
			OS::AssertMessage(result != GLFW_FALSE, "GLFW was not initialized.");
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			window_ = glfwCreateWindow(size.GetX(), size.GetY(), "Vulkan", nullptr, nullptr);
			OS::AssertMessage(window_ != nullptr, "GLFW Windows was not created.");
			glfwSetWindowUserPointer(window_, this);
			glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				Key keyboardKey;
				Event event;

				if (key == GLFW_KEY_W && action == GLFW_PRESS) {
					keyboardKey = Key::W;
					event = Event::Pressed;
				}
				else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
				keyboardKey = Key::S;
				event = Event::Pressed;
			}
			else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
				keyboardKey = Key::A;
				event = Event::Pressed;
			}
			else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
				keyboardKey = Key::D;
				event = Event::Pressed;
			}

				Window* thisWindow = (Window*)glfwGetWindowUserPointer(window);
			for (auto& callback : thisWindow->eventCallbacks_) {
				callback(keyboardKey, event);
			}
				});

			OS::LogInfo("/Window/", "GLFW Window  initialized successfuly.");*/

		}


		/*virtual */void SetTitle(const std::string& title) noexcept /*override */{
			glfwSetWindowTitle(window_, title.c_str());
		}

		void Show() {

		}

		void ProcessInput() {
			/*if (!glfwWindowShouldClose(window_)) {
				glfwPollEvents();
			}*/
		}

		//virtual void RegisterCallback(EventCallback&& eventCallback) noexcept {

		//	eventCallbacks_.push_back(eventCallback);

		//}

		~Window() {
			glfwDestroyWindow(window_);

			glfwTerminate();
			OS::LogInfo("/Window/", "GLFW Window  destroyed successfuly.");
		}

		//UIAL::Window::Info GetInfo() const override {

		//	UIAL::Window::Info info;
		//	{
		//		if (RAL::currentRenderAPI == RAL::RenderAPI::Vulkan) {

		//			info.nativeAPI_ = NativeAPI::GLFW;

		//			auto getRequiredExtensions = []()->Render::Vulkan::Extensions {

		//				uint32_t glfwExtensionCount = 0;
		//				const char** glfwExtensions = nullptr;

		//				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		//				Render::Vulkan::Extensions requiredExtensions;
		//				for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		//					const Render::Vulkan::Extension extension{ glfwExtensions[i] };
		//					requiredExtensions.AddExtension(extension);
		//				}

		//				return requiredExtensions;
		//			};


		//			Render::Vulkan::UIRequiredInfo requiredInfo;
		//			{
		//				requiredInfo.requiredExtensions_ = getRequiredExtensions();
		//				requiredInfo.windowHandle_ = glfwGetWin32Window(window_);
		//				requiredInfo.instanceHandle_ = GetModuleHandle(nullptr);

		//				int width, height;
		//				glfwGetFramebufferSize(window_, &width, &height);
		//				const Size frameBufferSize{ width, height };
		//				requiredInfo.frameBufferSize_ = frameBufferSize;
		//			}
		//			info.info_ = requiredInfo;
		//		}

		//	}
		//	return info;
		//}

		/*Size GetSize() const { return size_; }*/

		/*NativeAPI GetNativeType() const override { return NativeAPI::GLFW; }*/

	private:
		static void ErrorCallback(int error_code, const char* description) {
			OS::AssertFailMessage(description);
		}
	private:
		std::vector<EventCallback> eventCallbacks_;
		//UIAL::Window::Size size_;
		CreateInfo createInfo_;
		GLFWwindow* window_ = nullptr;
	};

	class API /*: public UIAL::API*/ {
	public:
#undef CreateWindow
		std::shared_ptr<Window> CreateWindow(const Window::CreateInfo& createInfo) const {
			return std::make_shared<Window>(createInfo);
		}

//
//		std::shared_ptr<UIAL::Window> CreateWindow(const UIAL::Window::CreateInfo& info) const override {
//			return std::make_shared<Window>(info.windowSize_);
//		}


	};

}