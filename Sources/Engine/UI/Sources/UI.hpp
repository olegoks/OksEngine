#pragma once 

#include <memory>
#include <string>
#include <vector>
#include <string>
#include <any>
#include <functional>

#include <Math.Vector.hpp>

namespace UIAL {

	class Window {
	public:

		virtual void SetTitle(const std::string& title) noexcept = 0;
		virtual void Show() = 0;
		
		virtual void ProcessInput() = 0;

		virtual void RegisterCallback(EventCallback&& eventCallback) noexcept = 0;

		virtual Size GetSize() const = 0;
		virtual ~Window() = default;

	private:

	};

	class API {
	public:

		virtual std::shared_ptr<Window> 
			CreateWindow(const Window::CreateInfo& info) const = 0;
	};

	
	std::unique_ptr<API> CreateAPIObject();

}