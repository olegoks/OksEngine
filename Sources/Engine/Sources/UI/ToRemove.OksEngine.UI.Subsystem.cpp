


namespace OksEngine {

	//UISubsystem::UISubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::UI, createInfo.context_ } {

	//	auto& context = GetContext();
	//	auto config = context.GetConfig();
	//	//auto world = context.GetECSWorld();
	//	//ECS::Entity::Id ui = world->CreateEntity();

	//	//api_ = std::make_shared<UI::API>();
	//	//UI::Window::CreateInfo windowCreateInfo;
	//	//{
	//	//	windowCreateInfo.title_ = config->GetValueAs<std::string>("UI.Window.Title");
	//	//	windowCreateInfo.windowSize_ = {
	//	//		config->GetValueAs<int>("UI.Window.Size.StartWidth"),
	//	//		config->GetValueAs<int>("UI.Window.Size.StartHeight")
	//	//	};
	//	//}
	//	//window_ = api_->CreateWindow(windowCreateInfo);
	//	//world->CreateComponent<Window>(ui, window_);
	//	//world->CreateComponent<KeyboardInput>(ui);
	//	//world->CreateComponent<MouseInput>(ui);
	//	//world->CreateComponent<FrameBufferResizeEvents>(ui);
	//	//context.GetECSWorld()->RegisterSystem<WindowSystem>(context);
	//	////context.GetECSWorld()->RegisterSystem<UISystem>(context);
	//	//context.GetECSWorld()->RegisterSystem<GetWindowFramebufferResizedEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<GetWindowKeyboardInputEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<CleanWindowKeyboardInputEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<SendWindowKeyboardInputEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<GetWindowMouseInputEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<CleanWindowMouseInputEvents>(context);
	//	//context.GetECSWorld()->RegisterSystem<SendWindowMouseInputEvents>(context);

	//	//window_->RegisterKeyboardEventCallback([this](UI::Window::Key key, UI::Window::Event event) {
	//	//		ProcessEvent(key, event);
	//	//	});
	//	//window_->RegisterMouseEventCallback([this](const Math::Vector2d& position) {
	//	//		ProcessMouseEvent(position);
	//	//	});
	//	//window_->Show();

	//}

	////void UISubsystem::ProcessEvent(UI::Window::Key key, UI::Window::Event event) {
	////	events_.push_back({ key, event });
	////}

	////void UISubsystem::ProcessMouseEvent(const Math::Vector2d& position) {
	////	mouseEvents_.push_back(position);
	////}


	//std::shared_ptr<UI::Window> UISubsystem::GetWindow() const noexcept {
	//	OS::AssertMessage(window_ != nullptr,
	//		"Attempt to get window pointer that was not initialized.");
	//	return window_;
	//}

	//void UISubsystem::Update() {
	//	
	//	window_->ProcessInput();
	//}

}