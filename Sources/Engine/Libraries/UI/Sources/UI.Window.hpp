#pragma once 

#include <memory>
#include <any>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <queue>

#include <UI.hpp>

#include <UIAL.Window.hpp>

namespace UI {


	class Window: public UIAL::Window  {
	public:

		Window(const CreateInfo& createInfo);

		virtual void DisableCursor() override;

		virtual void EnableCursor() override;
		
		[[nodiscard]]
		Info GetInfo(UIAL::Render render) const noexcept;

		[[nodiscard]]
		virtual glm::u32vec2 GetSize() const noexcept override;

		virtual void SetTitle(const std::string& title) noexcept override;

		virtual void Show() override;

		virtual void ProcessInput() override;

		~Window();

	private:
		static void ErrorCallback(int error_code, const char* description) {
			ASSERT_FAIL_MSG(description);
		}

	private:
		class GLFWwindow* window_ = nullptr;

	};

	class API /*: public UIAL::API*/ {
	public:
#undef CreateWindow
		std::shared_ptr<Window> CreateWindow(const Window::CreateInfo& createInfo) const {
			return std::make_shared<Window>(createInfo);
		}
	};


}