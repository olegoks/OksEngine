#pragma once
#include <UI\MainWindow\auto_OksEngine.MainWindow.hpp>

namespace OksEngine
{
#undef CreateWindow

	void CreateMainWindow::Update(ECS2::Entity::Id entity0id, const Config* config0, const LuaScript* luaScript0) {


		::Lua::Context context;

		::Lua::Script script{ luaScript0->text_ };
		context.LoadScript(script);

		auto api = std::make_shared<UI::API>();
		UI::Window::CreateInfo windowCreateInfo;
		{
			windowCreateInfo.title_ = context.GetGlobalAs<std::string>("UI.Window.Title");
			windowCreateInfo.windowSize_ = {
				context.GetGlobalAs<int>("UI.Window.Size.StartWidth"),
				context.GetGlobalAs<int>("UI.Window.Size.StartHeight")
			};
		}
		auto window = api->CreateWindow(windowCreateInfo);

		const ECS2::Entity::Id mainWindowEntity = CreateEntity();

		CreateComponent<MainWindow>(mainWindowEntity, window);
		CreateComponent<Input::KeyboardEvents>(mainWindowEntity);
		CreateComponent<MouseEvents>(mainWindowEntity);
		CreateComponent<CursorEvents>(mainWindowEntity);
		CreateComponent<MainWindowFramebufferResizeEvent>(mainWindowEntity);

		const glm::u32vec2 workAreaSize = window->GetWorkAreaSize();
		CreateComponent<MainWindowWorkAreaSize>(mainWindowEntity, workAreaSize.x, workAreaSize.y);

		const glm::u32vec2 frameBufferSize = window->GetFramebufferSize();
		CreateComponent<MainWindowFramebufferSize>(mainWindowEntity, frameBufferSize.x, frameBufferSize.y);

	};

	void ProcessInput::Update(ECS2::Entity::Id entity0id, MainWindow* mainWindow0) {

		mainWindow0->window_->ProcessInput();

	};

	void GetWindowFramebufferResizeEvents::Update(
		ECS2::Entity::Id entity0id, const OksEngine::MainWindow* mainWindow0,
		OksEngine::MainWindowFramebufferResizeEvent* mainWindowFramebufferResizeEvent0,
		OksEngine::MainWindowFramebufferSize* mainWindowFramebufferSize0) {

		std::optional<UIAL::Window::FrameBufferResizeEvent> maybeEvent = mainWindow0->window_->GetFrameBufferResizeEvent();
		if (maybeEvent.has_value()) {
			mainWindowFramebufferResizeEvent0->events_.push(maybeEvent.value());
		}
	}

	void ClearWindowFramebufferResizeEvents::Update(ECS2::Entity::Id entity0id, const MainWindow* mainWindow0,
		MainWindowFramebufferResizeEvent* mainWindowResizeEvent0) {

		while (!mainWindowResizeEvent0->events_.empty()) {
			mainWindowResizeEvent0->events_.pop();
		}
		
		
	}

	void GetWindowWorkAreaResizeEvents::Update(
		ECS2::Entity::Id entity0id, const OksEngine::MainWindow* mainWindow0,
		OksEngine::MainWindowWorkAreaResizeEvent* mainWindowWorkAreaResizeEvent0,
		OksEngine::MainWindowWorkAreaSize* mainWindowWorkAreaSize0) {

		std::optional<UIAL::Window::WorkAreaResizeEvent> maybeEvent = mainWindow0->window_->GetWorkAreaResizeEvent();
		if (maybeEvent.has_value()) {
			mainWindowWorkAreaResizeEvent0->events_.push(maybeEvent.value());
		}
	}

	void ClearWindowWorkAreaResizeEvents::Update(ECS2::Entity::Id entity0id, const OksEngine::MainWindow* mainWindow0,
		OksEngine::MainWindowWorkAreaResizeEvent* mainWindowWorkAreaResizeEvent0) {

		while (!mainWindowWorkAreaResizeEvent0->events_.empty()) {
			mainWindowWorkAreaResizeEvent0->events_.pop();
		}


	}

} // namespace OksEngine