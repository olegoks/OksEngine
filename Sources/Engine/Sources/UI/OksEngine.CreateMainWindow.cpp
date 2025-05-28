#include <UI\auto_OksEngine.CreateMainWindow.hpp>

#include <UI.Window.hpp>

namespace OksEngine {
void CreateMainWindow::Update(ECS2::Entity::Id entityId, const Config* config,
    const LuaScript* luaScript) {
    
	::Lua::Context context;

	::Lua::Script script{ luaScript->text_ };
	context.LoadScript(script);

	::Lua::Context context_;

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

};
}