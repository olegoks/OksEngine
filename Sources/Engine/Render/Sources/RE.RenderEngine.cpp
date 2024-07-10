
#include <RE.RenderEngine.hpp>
#include <RAL.API.hpp>
#include <Geometry.Texture.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>


#include <implot.h>
#include <implot_internal.h>

namespace RE {

	RenderEngine::RenderEngine(const RenderEngine::CreateInfo& createInfo) {

		api_ = RAL::CreateAPI();

		auto imguiPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"IMGUI",
			createInfo.imguiVertexShader_,
			createInfo.imguiFragmentShader_,
			true
		);

		auto linesPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"Lines",
			createInfo.linesVertexShader_,
			createInfo.linesFragmentShader_,
			true
		);

		auto flatShadedModelPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"Flat shading models",
			createInfo.vertexShader_,
			createInfo.fragmentShader_,
			true
		);

		auto texturedModelPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"Textured models",
			createInfo.textureVertexShader_,
			createInfo.textureFragmentShader_,
			true
		);

		RAL::Driver::CreateInfo driverCreateInfo;
		{
			driverCreateInfo.imguiPipeline_ = imguiPipeline;
			driverCreateInfo.linesPipeline_ = linesPipeline;
			driverCreateInfo.flatShadedPipeline_ = flatShadedModelPipeline;
			driverCreateInfo.texturedPipeline_ = texturedModelPipeline;

			driverCreateInfo.surface_ = *createInfo.renderSurface_;
		}

		driver_ = api_->CreateDriver(driverCreateInfo);
		driver_->SetCamera(createInfo.camera_);
		driver_->AddLight(createInfo.light_);
	}

	void RenderEngine::RenderImGui() {

		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		size_t upload_size = width * height * 4 * sizeof(char);

		Geom::Texture<Geom::Color4b>::CreateInfo textureCreateInfo{
			width,
			height,
			{ (Geom::Color4b*)pixels, (Common::Size)width * height }
		};

		auto texture = std::make_shared<Geom::Texture<Geom::Color4b>>(textureCreateInfo);

		//ImGui_ImplVulkan_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//{
		//	bool isOpen = true;
		//	ImGui::Begin("Menu", &isOpen, ImGuiWindowFlags_MenuBar);
		//	ImGui::BeginMenuBar();
		//	// Add items to the menu bar.
		//	ImGui::MenuItem("File", NULL, false, false);
		//	ImGui::MenuItem("Edit", NULL, false, false);
		//	ImGui::MenuItem("View", NULL, false, false);
		//	ImGui::MenuItem("Help", NULL, false, false);
		//	// End the menu bar.
		//	ImGui::EndMenuBar();
		//	ImGui::End();
		//}
		//{
		//	bool isOpen = true;
		//	ImGui::Begin("Engine performance", &isOpen, 0);


		//	static Common::UInt64 renderCalls = 0;
		//	++renderCalls;

		//	using namespace std::chrono_literals;
		//	std::chrono::high_resolution_clock::time_point now;
		//	static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

		//	now = std::chrono::high_resolution_clock::now();
		//	auto delta = now - point;

		//	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
		//	static Common::Size lastFps = 0;
		//	static std::vector<Common::Size> fps_;
		//	static std::vector<Common::Size> timePoints_;
		//	if (microseconds > 1000000) {
		//		Common::Size framesPerSecond = renderCalls * 1000000 / microseconds;
		//		ImGui::TextDisabled("Fps: %d", framesPerSecond);
		//		fps_.push_back(framesPerSecond);
		//		timePoints_.push_back(fps_.size());
		//		renderCalls = 0;
		//		point = now;
		//		lastFps = framesPerSecond;

		//	}
		//	else {
		//		ImGui::TextDisabled("Fps: %d", lastFps);
		//	}
		//	//int   bar_data[11] = {10, 11, 5, 6,1, 7 , 10, 11, 5, 6,1 };

		//	ImGui::Begin("My Window");
		//	//const ImVec2  size{20, 1000};
		//	if (ImPlot::BeginPlot("My Plot"/*, size*/)) {
		//		//ImPlot::PlotBars("My Bar Plot", bar_data, 11);
		//		//const Common::Size timePoint = fps_.size();
		//		ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
		//		ImPlot::EndPlot();
		//	}
		//	ImGui::End();
		//	ImGui::End();
		//}

		//ImGui::Render();

		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer, 0);

	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const glm::mat4& position, const Geometry::Model<RAL::Vertex3fnt, RAL::Index16>& model) {

		Model drawnModel;
		for (const auto& shape : model) {
			Geometry::VertexCloud<Geometry::Vertex3fnt> verticesColored;
			const auto& vertices = shape.GetVertices();
			for (const auto& vertex : vertices) {
				verticesColored.Add(vertex);
			}
			Common::Index shapeId = driver_->DrawIndexed(
				position,
				(const RAL::Vertex3fnt*)verticesColored.GetData(),
				verticesColored.GetVerticesNumber(),
				shape.GetIndices().GetData(),
				shape.GetIndicesNumber(),
				shape.GetTexture()
			);
			drawnModel.shapes_.push_back({ shapeId });
		}

		return drawnModel;
	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const glm::mat4& position, const Geometry::Model<RAL::Vertex3fnc, RAL::Index16>& model) {
		Model drawnModel;
		for (const auto& shape : model) {
			Geometry::VertexCloud<Geometry::Vertex3fnc> verticesColored;

			const auto& vertices = shape.GetVertices();
			for (const auto& vertex : vertices) {
				verticesColored.Add(vertex);
			}
			Common::Index shapeId = driver_->DrawIndexed(
				position,
				(RAL::Vertex3fnc*)verticesColored.GetData(),
				verticesColored.GetVerticesNumber(),
				shape.GetIndices().GetData(),
				shape.GetIndicesNumber());
			drawnModel.shapes_.push_back({ shapeId });
		}
		return drawnModel;
	}

 


	void RenderEngine::SetModelMatrix(const RenderEngine::Model& model, const glm::mat4& modelMatrix) {
		for (auto shape : model.shapes_) {
			driver_->SetModelMatrix(shape.id_, modelMatrix);
		}
	}

	void RenderEngine::Render() {
		//Geometry::Box box{ 1 };

		//Geometry::VertexCloud<RAL::Vertex3fc> coloredBox;
		//for (Common::Index i = 0; i < box.GetVertices().GetVerticesNumber(); i++) {
		//	Math::Vector3f color{ (float)((i * 100) % 255) / 255, (float)((i * 150) % 255) / 255, (float)((i * 199) % 255) / 255 };
		//	RAL::Vertex3fc coloredVertex = { box.GetVertices()[i], color };
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

		driver_->StartRender();
		driver_->Render();
		driver_->EndRender();


		using namespace std::chrono_literals;
		std::chrono::high_resolution_clock::time_point now;
		static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

		now = std::chrono::high_resolution_clock::now();
		auto delta = now - point;
		point = now;

		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
		Common::Size framesPerSecond = Common::Limits<Common::Size>::Max();
		if (milliseconds != 0) {
			framesPerSecond = 1000 / milliseconds;
		}
		//OS::LogInfo("renderEngine", { "Frames per second {}", framesPerSecond });
	}

	void RenderEngine::SetCamera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
		auto camera = driver_->GetCamera();
		camera->SetPosition(position);
		camera->SetDirection(direction);
		camera->SetUp(up);
		driver_->SetCamera(camera);
	}

}