
#include <Render/OksEngine.Render.Subsystem.hpp>

#include <Render/OksEngine.AddImmutableRenderGeometryFromObjRequest.hpp>

namespace OksEngine {


	RenderSubsystem::RenderSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Render, createInfo.context_ } {

		auto& context = GetContext();
		auto config = context.GetConfig();

		auto resourceSubsystem = context.GetResourceSubsystem();

		const auto imguiVertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/imgui.vert");
		const auto imguiFragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/imgui.frag");

		const auto linesVertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/debugLines.vert");
		const auto linesFragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/debugLines.frag");

		const auto vertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/flatShaded.vert");
		const auto fragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/flatShaded.frag");

		const auto vertexTextureShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/textured.vert");
		const auto fragmentTextureShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/textured.frag");


		auto ecsWorld = context.GetECSWorld();
		ecsWorld->RegisterSystem<CreateLoadGeometryDescriptionFileRequest>(context);
		ecsWorld->RegisterSystem<CreateLoadMeshRequest>(context);
		ecsWorld->RegisterSystem<RenderSystem>(context);
		ecsWorld->RegisterSystem<CameraSystem>(context);
		ecsWorld->RegisterSystem<CreateDriverCamera>(context);
		ecsWorld->RegisterSystem<CreateDriverModel>(context);
		ecsWorld->RegisterSystem<MapMeshTransform>(context);
		ecsWorld->RegisterSystem<LoadGeometryDescriptionFile>(context);
		ecsWorld->RegisterSystem<LoadMesh>(context);
		ecsWorld->RegisterSystem<RenderMesh>(context);
		ecsWorld->RegisterSystem<AddImmutableRenderGeometryFromObjSystem>(context);
		auto uiSubsystem = context.GetUISubsystem();
		auto windowInfo = uiSubsystem->GetWindow()->GetInfo(UIAL::Render::Vulkan);
		RAL::RenderSurface renderSurface;
		if (windowInfo.subsystem_ == UIAL::Subsystem::GLFW) {

			renderSurface.param1_ = windowInfo.param1_;
			renderSurface.param2_ = windowInfo.param2_;
			renderSurface.param3_ = windowInfo.param3_;
			renderSurface.size_ = windowInfo.size_;
			OS::AssertMessage(renderSurface.size_.x == config->GetValueAs<Common::Size>("UI.Window.Size.StartWidth"),
				"Start size in config and got from ui system are different.");
			OS::AssertMessage(renderSurface.size_.y == config->GetValueAs<Common::Size>("UI.Window.Size.StartHeight"),
				"Start size in config and got from ui system are different.");
			renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;
		}

		//RAL::Camera::CreateInfo cameraCreateInfo;
		//{
		//	cameraCreateInfo.position_ = { 5.f, 0.f, 0.f };
		//	cameraCreateInfo.direction_ = glm::vec3{ 0.f, 0.f, 0.f } - cameraCreateInfo.position_;
		//	cameraCreateInfo.zFar_ = 10000000.f;
		//	cameraCreateInfo.size_ = windowInfo.size_;
		//}

		//auto camera = std::make_shared<RAL::Camera>(cameraCreateInfo);

		//RAL::Light::CreateInfo lightCreateInfo;
		//{
		//	lightCreateInfo.intensity_ = 1.f;
		//	lightCreateInfo.position_ = { 25.f, 0.f, 0.f };//camera->GetPosition();
		//}
		//auto light = std::make_shared<RAL::Light>(lightCreateInfo);
		ResourceSubsystem::Resource imguiVertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, imguiVertexShaderTaskId);
		ResourceSubsystem::Resource imguiFragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, imguiFragmentShaderTaskId);
		ResourceSubsystem::Resource linesVertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, linesVertexShaderTaskId);
		ResourceSubsystem::Resource linesFragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, linesFragmentShaderTaskId);
		ResourceSubsystem::Resource vertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexShaderTaskId);
		ResourceSubsystem::Resource fragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentShaderTaskId);
		ResourceSubsystem::Resource vertexTextureShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexTextureShaderTaskId);
		ResourceSubsystem::Resource fragmentTextureShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentTextureShaderTaskId);

		std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
		std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };
		std::string linesVertexShader{ linesVertexShaderResource.GetData<Common::Byte>(), linesVertexShaderResource.GetSize() };
		std::string linesFragmentShader{ linesFragmentShaderResource.GetData<Common::Byte>(), linesFragmentShaderResource.GetSize() };
		std::string vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
		std::string fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };
		std::string vertexTextureShader{ vertexTextureShaderResource.GetData<Common::Byte>(), vertexTextureShaderResource.GetSize() };
		std::string fragmentTextureShader{ fragmentTextureShaderResource.GetData<Common::Byte>(), fragmentTextureShaderResource.GetSize() };

		RE::RenderEngine::CreateInfo RECreateInfo{
			//.camera_ = camera,
			//.light_ = light,
			.imguiVertexShader_ = std::move(imguiVertexShader),
			.imguiFragmentShader_ = std::move(imguiFragmentShader),
			.linesVertexShader_ = std::move(linesVertexShader),
			.linesFragmentShader_ = std::move(linesFragmentShader),
			.vertexShader_ = std::move(vertexShader),
			.fragmentShader_ = std::move(fragmentShader),
			.textureVertexShader_ = std::move(vertexTextureShader),
			.textureFragmentShader_ = std::move(fragmentTextureShader),
			.renderSurface_ = std::make_shared<RAL::RenderSurface>(renderSurface)
		};
		engine_ = std::make_shared<RE::RenderEngine>(RECreateInfo);

		//const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/room.obj");
		//ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

		//{
		//	const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/skelet 6 blend.obj");
		//	ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

		//	const auto mtlTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/skelet 6 blend.mtl");
		//	ResourceSubsystem::Resource mtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlTaskId);

		//	std::string obj{ modelResource.GetData<char>(), modelResource.GetSize() };
		//	std::string mtl{ mtlResource.GetData<char>(), mtlResource.GetSize() };

		//	auto model = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));
		//	//skeleton_ = engine_->RenderModel({ 0, 0, 0 }, *model);
		//}

		//{
		//	const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/Love.obj");
		//	ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

		//	const auto mtlTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/Love.mtl");
		//	ResourceSubsystem::Resource mtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlTaskId);

		//	std::string obj{ modelResource.GetData<char>(), modelResource.GetSize() };
		//	std::string mtl{ mtlResource.GetData<char>(), mtlResource.GetSize() };

		//	auto model = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));
		//	//engine_->RenderModel(*model_);
		//}
		//{
		//}
	}

	//[[nodiscard]]
	//Common::Index RenderSubsystem::RenderModel(std::string objName, std::string mtlName) {

	//	auto& context = GetContext();
	//	auto resourceSubsystem = context.GetResourceSubsystem();


	//	const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + objName);
	//	ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);

	//	const auto mtlBlockTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + mtlName);
	//	ResourceSubsystem::Resource mtlCubeMtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockTaskId);

	//	std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
	//	std::string mtl{ mtlCubeMtlResource.GetData<char>(), mtlCubeMtlResource.GetSize() };

	//	auto flatShadedModel = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));

	//	RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4{ 0 }, *flatShadedModel);
	//	models_.push_back(model);
	//	return models_.size() - 1;
	//}

	//[[nodiscard]]
	//Common::Index RenderSubsystem::RenderAnimationModel(const std::vector<std::string>& filesNames) {

	//	auto& context = GetContext();
	//	auto resourceSubsystem = context.GetResourceSubsystem();
	//	if (filesNames.size() == 1 && std::filesystem::path(filesNames[0]).extension() == ".fbx") {
	//		const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + filesNames[0]);
	//		ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);
	//		Geom::ParseModelFbx(modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize());

	//	}

	//	
	//	
	//	//Geom::ParseModelFile("G:/Desktop/OksEngine/Resources/Models/myLowPolyHand/myLowPolyHand.fbx");
	//	//auto texturedModel = std::make_shared<Geom::Model<Geom::Vertex3fnt, Geom::Index16>>(Geometry::ParseModel(obj));

	//	//RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4(), *texturedModel);
	//	//models_.push_back(model);
	//	return 0;/*models_.size() - 1;*/
	//}

	//[[nodiscard]]
	//Common::Index RenderSubsystem::RenderModel(std::string objName, std::string mtlName, const std::vector<std::string>& textures) {

	//	auto& context = GetContext();
	//	auto resourceSubsystem = context.GetResourceSubsystem();


	//	const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + objName);
	//	ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);

	//	const auto mtlBlockTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + mtlName);
	//	ResourceSubsystem::Resource mtlCubeMtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockTaskId);

	//	std::vector<ResourceSubsystem::Resource> texturesResources;
	//	for (const std::string& textureName : textures) {

	//		const auto mtlBlockPngTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + textureName);
	//		texturesResources.push_back(resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockPngTaskId));
	//	}

	//	std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
	//	std::string mtl{ mtlCubeMtlResource.GetData<char>(), mtlCubeMtlResource.GetSize() };
	//	std::string image{ texturesResources[0].GetData<char>(), texturesResources[0].GetSize()};

	//	std::vector<RAL::Texture::Id> texturesIds;


	//	auto texturedModel = std::make_shared<Geom::Model<Geom::Vertex3fnt, Geom::Index16>>(Geometry::ParseObjVertex3fntIndex16Textures(obj, mtl));
	//	for (auto& shape : *texturedModel) {
	//		
	//	}
	//	RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4(), *texturedModel);
	//	models_.push_back(model);
	//	return models_.size() - 1;
	//}

	//[[nodiscard]]
	//Common::Index RenderSubsystem::RenderModelTextures(std::string objName, std::string mtlName) {

	//	//auto& context = GetContext();
	//	//auto resourceSubsystem = context.GetResourceSubsystem();


	//	//const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + objName);
	//	//ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);

	//	//const auto mtlBlockTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + mtlName);
	//	//ResourceSubsystem::Resource mtlCubeMtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockTaskId);

	//	//std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
	//	//std::string mtl{ mtlCubeMtlResource.GetData<char>(), mtlCubeMtlResource.GetSize() };

	//	//auto texturedModel = std::make_shared<Geom::Model<Geom::Vertex3fnt, Geom::Index16>>(Geometry::ParseObjVertex3fntIndex16Textures(obj, mtl));
	//	//for (auto& shape : *texturedModel) {
	//	//	const auto mtlBlockPngTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + shape.textureName_);
	//	//	ResourceSubsystem::Resource resource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockPngTaskId);
	//	//	std::string texture{ resource.GetData<char>(), resource.GetSize() };
	//	//	
	//	//	shape.SetTexture(std::make_shared<Geom::Texture<Geom::Color4b>>(std::move(Geom::CreateTexture(texture.data(), texture.size()))));
	//	//}
	//	//RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4(), *texturedModel);
	//	//models_.push_back(model);
	//	return 0;//models_.size() - 1;
	//}

	//void RenderSubsystem::SetCamera(const glm::vec3& position, const glm::vec3& direction, const glm:: vec3& up) {
	//	engine_->SetCamera(position, direction, up);
	//}

	void RenderSubsystem::Update() noexcept {
		//Geometry::Box box{ 1 };

		//Geometry::VertexCloud<RAL::Vertex3fc> coloredBox;
		//for (Common::Index i = 0; i < box.GetVertices().GetVerticesNumber(); i++) {
		//	Math::Vector3f color{ (float)((i * 100) % 255) / 255, (float)((i * 150) % 255) /255, (float)((i * 199) % 255) / 255 };
		//	RAL::Vertex3fc coloredVertex = {box.GetVertices()[i], color };
		//	coloredBox.Add(coloredVertex);
		//}

		//DS::Vector<Geometry::VertexCloud<RAL::Vertex3fc>> plane;

		//for (int i = 0; i < 25; i++) {
		//	plane.PushBack(coloredBox);
		//}


		//Common::Index i = 0;
		//for (int x = -2; x < 2; x++) {
		//	for (int y = -2; y < 2; y++) {
		//		Geometry::VertexCloud<RAL::Vertex3fc>& box = plane[i];
		//		Math::Vector3f offsetVector{ (float)x, (float)y , 0 };
		//		const Math::Matrix4x4f offset = Math::Matrix4x4f::GetTranslate(offsetVector);
		//		for (RAL::Vertex3fc& vertex : box) {
		//			vertex.position_ = Math::TransformPoint(vertex.position_, offset);
		//		}
		//		++i;
		//	}
		//}

		//for (int i = 0; i < 25; i++) {
		//	Geometry::VertexCloud<RAL::Vertex3fc>& coloredBox = plane[i];
		//	driver_->DrawIndexed(
		//		(RAL::Vertex3fc*)coloredBox.GetData(),
		//		coloredBox.GetVerticesNumber(),
		//		box.GetIndices().GetData(),
		//		box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);
		//}
		//
		//

		////driver_->DrawIndexed(
		////	(RAL::Vertex3fc*)coloredBox.GetData(),
		////	coloredBox.GetVerticesNumber(),
		////	box.GetIndices().GetData(),
		////	box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);

		//driver_->StartRender();
		//driver_->Render();
		//driver_->EndRender();

		//engine_->RotateModel(dragonLore_, { 1, 0, 0 }, 1);
		//engine_->RotateModel(skeleton_, { 0, 1, 0 }, 1);

		
		//for (auto& callback : imguiCallbacks_) {
		//	callback();
		//}
	//	if (ImGui::BeginMainMenuBar()) {
	//				// Add items to the menu bar.
	//	if (ImGui::BeginMenu("Engine")) {
	//		static bool showPerformanceProfiler_ = false;
	//		ImGui::MenuItem("Performance profiler", nullptr, &showPerformanceProfiler_);

	//		ImGui::MenuItem("ECS ", NULL, false, false);
	//		ImGui::EndMenu();
	//	}
	//	// End the menu bar.
	//	ImGui::EndMainMenuBar();
	//}
	//if (ImGui::BeginMainMenuBar()) {
	//	// Add items to the menu bar.
	//	if (ImGui::BeginMenu("Engine")) {
	//		ImGui::MenuItem("Render", NULL, false, false);
	//		ImGui::MenuItem("Help", NULL, false, false);
	//		ImGui::EndMenu();
	//	}
	//	// End the menu bar.
	//	ImGui::EndMainMenuBar();
	//}

	//	//imguiCallbacks_.clear();
		
		engine_->Render();
	}

	/*void RenderSubsystem::RenderImGui()
	{
		engine_->RenderImGui();
	}*/

	//void RenderSubsystem::AddImGuiCallback(ImGuiCallback&& imguiCallback) {
	//	imguiCallbacks_.push_back(std::move(imguiCallback));
	//}

	//void RenderSubsystem::UpdateCamera(Camera* camera) {

	//}

	//void RenderSubsystem::UpdateImmutableRenderGeometry(ImmutableRenderGeometry* immutableRenderGeometry) {

	//}

}