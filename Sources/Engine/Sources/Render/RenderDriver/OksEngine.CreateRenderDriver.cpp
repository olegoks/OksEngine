#include <Render/RenderDriver\auto_OksEngine.CreateRenderDriver.hpp>

namespace OksEngine {
void CreateRenderDriver::Update(
    ECS2::Entity::Id entityId1, const MainWindow* mainWindow,
    ECS2::Entity::Id entityId2, const Config* config,
    const LuaScript* luaScript, ECS2::Entity::Id entityId,
    ResourceSystem* resourceSystem) {


	Resources::ResourceData imguiVertexShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/imgui.vert");
	Resources::ResourceData imguiFragmentShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/imgui.frag");
	Resources::ResourceData linesVertexShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/debugLines.vert");
	Resources::ResourceData linesFragmentShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/debugLines.frag");
	Resources::ResourceData vertexShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/flatShaded.vert");
	Resources::ResourceData fragmentShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/flatShaded.frag");
	Resources::ResourceData vertexTextureShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/textured.vert");
	Resources::ResourceData fragmentTextureShaderResource = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Render, "Root/textured.frag");

	std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
	std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };
	std::string linesVertexShader{ linesVertexShaderResource.GetData<Common::Byte>(), linesVertexShaderResource.GetSize() };
	std::string linesFragmentShader{ linesFragmentShaderResource.GetData<Common::Byte>(), linesFragmentShaderResource.GetSize() };
	std::string vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
	std::string fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };
	std::string vertexTextureShader{ vertexTextureShaderResource.GetData<Common::Byte>(), vertexTextureShaderResource.GetSize() };
	std::string fragmentTextureShader{ fragmentTextureShaderResource.GetData<Common::Byte>(), fragmentTextureShaderResource.GetSize() };


	auto api = RAL::CreateAPI();

	RAL::Driver::CreateInfo driverCreateInfo{};

	{

		auto windowInfo = mainWindow->window_->GetInfo(UIAL::Render::Vulkan);
		RAL::RenderSurface renderSurface;
		if (windowInfo.subsystem_ == UIAL::Subsystem::GLFW) {

			renderSurface.param1_ = windowInfo.param1_;
			renderSurface.param2_ = windowInfo.param2_;
			renderSurface.param3_ = windowInfo.param3_;
			renderSurface.size_ = windowInfo.size_;
			renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;
		}

		auto renderSurfacePtr = std::make_shared<RAL::RenderSurface>(renderSurface);

		driverCreateInfo.surface_ = *renderSurfacePtr;
	}


	



	{
		RAL::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "ImGuiVertexShader",
			.type_ = RAL::Shader::Type::Vertex,
			.code_ = imguiVertexShader
		};
		auto vertexShader = api->CreateShader(vertexShaderCreateInfo);
		RAL::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "ImGuiFragmentShader",
			.type_ = RAL::Shader::Type::Fragment,
			.code_ = imguiFragmentShader
		};
		auto fragmentShader = api->CreateShader(fragmentShaderCreateInfo);
		//driverCreateInfo.imguiPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
		//	"ImGui",
		//	vertexShader,
		//	fragmentShader);

		std::vector<RAL::Driver::ShaderBinding::Layout> shaderBindings;

		RAL::Driver::ShaderBinding::Layout transformBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader
		};

		RAL::Driver::ShaderBinding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
			.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader
		};

		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::PipelineDescription pipelineDesc{
			.name_ = "ImGui Pipeline",
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::TopologyType::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::None,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = true,
			.dbCompareOperation_ = RAL::Driver::DepthBuffer::CompareOperation::Always

		};
		driverCreateInfo.namePipelineDescriptions_["ImGui Pipeline"] = pipelineDesc;


	}


	{
		RAL::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "TexturedVertexShader",
			.type_ = RAL::Shader::Type::Vertex,
			.code_ = vertexTextureShader
		};
		auto vertexShader = api->CreateShader(vertexShaderCreateInfo);
		RAL::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "TexturedFragmentShader",
			.type_ = RAL::Shader::Type::Fragment,
			.code_ = fragmentTextureShader
		};
		auto fragmentShader = api->CreateShader(fragmentShaderCreateInfo);
		//driverCreateInfo.imguiPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
		//	"Textured",
		//	vertexShader,
		//	fragmentShader);

		std::vector<RAL::Driver::ShaderBinding::Layout> shaderBindings;

		RAL::Driver::ShaderBinding::Layout cameraBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader
		};

		RAL::Driver::ShaderBinding::Layout transformBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader
		};

		RAL::Driver::ShaderBinding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
			.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader
		};

		shaderBindings.push_back(cameraBinding);
		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::PipelineDescription pipelineDesc{
			.name_ = "Textured Pipeline",
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::TopologyType::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2,
			.indexType_ = RAL::Driver::IndexType::UI16,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::Back,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = true,
			.dbCompareOperation_ = RAL::Driver::DepthBuffer::CompareOperation::Less

		};
		driverCreateInfo.namePipelineDescriptions_["Textured Pipeline"] = pipelineDesc;


		auto driver = api->CreateDriver(driverCreateInfo);

		const ECS2::Entity::Id driverEntityId = CreateEntity();
		CreateComponent<RenderDriver>(driverEntityId, driver);
	}

};
}