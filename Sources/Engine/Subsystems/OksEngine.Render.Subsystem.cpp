
#include <OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	RenderSubsystem::RenderSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Render, createInfo.context_ } {

		auto& context = GetContext();
		auto config = context.GetConfig();

		auto resourceSubsystem = context.GetResourceSubsystem();

		const auto vertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleVert.spv");
		const auto fragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleFrag.spv");

		const auto vertexTextureShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleTextureVert.spv");
		const auto fragmentTextureShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleTextureFrag.spv");


		auto ecsWorld = context.GetECSWorld();
		ecsWorld->RegisterSystem<RenderSystem>(context);
		ecsWorld->RegisterSystem<CameraSystem>(context);

		auto uiSubsystem = context.GetUISubsystem();
		auto windowInfo = uiSubsystem->GetWindow()->GetInfo(UI::Render::Vulkan);
		RAL::RenderSurface renderSurface;
		if (windowInfo.subsystem_ == UI::Subsystem::GLFW) {

			renderSurface.param1_ = windowInfo.param1_;
			renderSurface.param2_ = windowInfo.param2_;
			renderSurface.param3_ = windowInfo.param3_;
			renderSurface.size_ = windowInfo.size_;
			OS::AssertMessage(renderSurface.size_.GetX() == config->GetValueAs<Common::Size>("UI.Window.Size.StartWidth"),
				"Start size in config and got from ui system are different.");
			OS::AssertMessage(renderSurface.size_.GetY() == config->GetValueAs<Common::Size>("UI.Window.Size.StartHeight"),
				"Start size in config and got from ui system are different.");
			renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;
		}

		RAL::Camera::CreateInfo cameraCreateInfo;
		{
			cameraCreateInfo.position_ = Math::Vector3f{ 5.f, 0.f, 0.f };
			cameraCreateInfo.direction_ = Math::Vector3f{ 0.f, 0.f, 0.f } - cameraCreateInfo.position_;
			cameraCreateInfo.size_ = windowInfo.size_;
		}

		auto camera = std::make_shared<RAL::Camera>(cameraCreateInfo);

		RAL::Light::CreateInfo lightCreateInfo;
		{
			lightCreateInfo.intensity_ = 1.f;
			lightCreateInfo.position_ = camera->GetPosition();
		}
		auto light = std::make_shared<RAL::Light>(lightCreateInfo);

		ResourceSubsystem::Resource vertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexShaderTaskId);
		ResourceSubsystem::Resource fragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentShaderTaskId);
		ResourceSubsystem::Resource vertexTextureShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexTextureShaderTaskId);
		ResourceSubsystem::Resource fragmentTextureShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentTextureShaderTaskId);

		RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
		RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };
		RAL::Shader vertexTextureShader{ vertexTextureShaderResource.GetData<Common::Byte>(), vertexTextureShaderResource.GetSize() };
		RAL::Shader fragmentTextureShader{ fragmentTextureShaderResource.GetData<Common::Byte>(), fragmentTextureShaderResource.GetSize() };

		RE::RenderEngine::CreateInfo RECreateInfo;
		{
			RECreateInfo.camera_ = camera;
			RECreateInfo.light_ = light;
			RECreateInfo.vertexShader_ = std::make_shared<RAL::Shader>(std::move(vertexShader));
			RECreateInfo.fragmentShader_ = std::make_shared<RAL::Shader>(std::move(fragmentShader));
			RECreateInfo.textureVertexShader_ = std::make_shared<RAL::Shader>(std::move(vertexTextureShader));
			RECreateInfo.textureFragmentShader_ = std::make_shared<RAL::Shader>(std::move(fragmentTextureShader));
			RECreateInfo.renderSurface_ = std::make_shared<RAL::RenderSurface>(std::move(renderSurface));
		}
		engine_ = std::make_shared<RE::RenderEngine>(RECreateInfo);

		//const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/room.obj");
		//ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

		{
			const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/skelet 6 blend.obj");
			ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

			const auto mtlTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/skelet 6 blend.mtl");
			ResourceSubsystem::Resource mtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlTaskId);

			std::string obj{ modelResource.GetData<char>(), modelResource.GetSize() };
			std::string mtl{ mtlResource.GetData<char>(), mtlResource.GetSize() };

			auto model = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));
			//skeleton_ = engine_->RenderModel({ 0, 0, 0 }, *model);
		}

		{
			const auto modelTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/Love.obj");
			ResourceSubsystem::Resource modelResource = resourceSubsystem->GetResource(Subsystem::Type::Render, modelTaskId);

			const auto mtlTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/Love.mtl");
			ResourceSubsystem::Resource mtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlTaskId);

			std::string obj{ modelResource.GetData<char>(), modelResource.GetSize() };
			std::string mtl{ mtlResource.GetData<char>(), mtlResource.GetSize() };

			auto model = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));
			//engine_->RenderModel(*model_);
		}
		{
		}
	}

	[[nodiscard]]
	Common::Index RenderSubsystem::RenderModel(std::string objName, std::string mtlName) {

		auto& context = GetContext();
		auto resourceSubsystem = context.GetResourceSubsystem();


		const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + objName);
		ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);

		const auto mtlBlockTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + mtlName);
		ResourceSubsystem::Resource mtlCubeMtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockTaskId);

		std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
		std::string mtl{ mtlCubeMtlResource.GetData<char>(), mtlCubeMtlResource.GetSize() };

		auto flatShadedModel = std::make_shared<Geom::Model<Geom::Vertex3fnc, Geom::Index16>>(Geometry::ParseObjVertex3fncIndex16(obj, mtl));

		RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4{0}, *flatShadedModel);
		models_.push_back(model);
		return models_.size() - 1;
	}

	[[nodiscard]]
	Common::Index RenderSubsystem::RenderModel(std::string objName, std::string mtlName, std::string textureName) {

		auto& context = GetContext();
		auto resourceSubsystem = context.GetResourceSubsystem();


		const auto blockModelObjTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + objName);
		ResourceSubsystem::Resource modelCubeObjResource = resourceSubsystem->GetResource(Subsystem::Type::Render, blockModelObjTaskId);

		const auto mtlBlockTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + mtlName);
		ResourceSubsystem::Resource mtlCubeMtlResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockTaskId);

		const auto mtlBlockPngTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/" + textureName);
		ResourceSubsystem::Resource pngCubeResource = resourceSubsystem->GetResource(Subsystem::Type::Render, mtlBlockPngTaskId);

		std::string obj{ modelCubeObjResource.GetData<char>(), modelCubeObjResource.GetSize() };
		std::string mtl{ mtlCubeMtlResource.GetData<char>(), mtlCubeMtlResource.GetSize() };
		std::string image{ pngCubeResource.GetData<char>(), pngCubeResource.GetSize() };


		auto texturedModel = std::make_shared<Geom::Model<Geom::Vertex3fnt, Geom::Index16>>(Geometry::ParseObjVertex3fntIndex16(obj, mtl, image));

		RE::RenderEngine::Model model = engine_->RenderModel(glm::mat4(), *texturedModel);
		models_.push_back(model);
		return models_.size() - 1;
	}

	void RenderSubsystem::SetCamera(const Math::Vector3f& position, const Math::Vector3f& direction) {
		engine_->SetCamera(position, direction);
	}

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
		engine_->Render();
	}


}