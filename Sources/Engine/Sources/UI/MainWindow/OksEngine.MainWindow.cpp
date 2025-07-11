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
		CreateComponent<KeyboardInput>(mainWindowEntity);
		CreateComponent<FrameBufferResizeEvents>(mainWindowEntity);

	};

	void ProcessInput::Update(ECS2::Entity::Id entity0id, MainWindow* mainWindow0) {

		mainWindow0->window_->ProcessInput();

	};

	void GetWindowResizeEvents::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		FrameBufferResizeEvents* frameBufferResizeEvents0) {

		std::optional<UIAL::Window::FrameBufferResizeEvent> maybeEvent = mainWindow0->window_->GetFrameBufferResizeEvent();
		if (maybeEvent.has_value()) {
			frameBufferResizeEvents0->events_.push(maybeEvent.value());
		}
	}

} // namespace OksEngine