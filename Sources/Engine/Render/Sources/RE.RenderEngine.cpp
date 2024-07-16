
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

#include <glm/glm.hpp>

namespace RE {

	RenderEngine::RenderEngine(const RenderEngine::CreateInfo& createInfo) {

		api_ = RAL::CreateAPI();


		auto imguiNativePipeline = std::make_shared<RAL::Driver::Pipeline>(
			"IMGUI native",
			createInfo.imguiNativeVertexShader_,
			createInfo.imguiNativeFragmentShader_,
			false
		);

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
			driverCreateInfo.imguiNativePipeline_ = imguiNativePipeline;
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
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		{
			bool isOpen = true;
			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem("Performance profiler", NULL, false, true);
					ImGui::MenuItem("ECS", NULL, false, false);
					ImGui::MenuItem("Render", NULL, false, false);
					ImGui::MenuItem("Help", NULL, false, false);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}
		}
		{
			bool isOpen = true;
			ImGui::Begin("Engine performance", &isOpen, 0);


			static Common::UInt64 renderCalls = 0;
			++renderCalls;

			using namespace std::chrono_literals;
			std::chrono::high_resolution_clock::time_point now;
			static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

			now = std::chrono::high_resolution_clock::now();
			auto delta = now - point;

			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
			static Common::Size lastFps = 0;
			static std::vector<Common::Size> fps_;
			static std::vector<Common::Size> timePoints_;
			if (microseconds > 1000000) {
				Common::Size framesPerSecond = renderCalls * 1000000 / microseconds;
				ImGui::TextDisabled("Fps: %d", framesPerSecond);
				fps_.push_back(framesPerSecond);
				timePoints_.push_back(fps_.size());
				renderCalls = 0;
				point = now;
				lastFps = framesPerSecond;

			}
			else {
				ImGui::TextDisabled("Fps: %d", lastFps);
			}

			ImGui::Begin("My Window");
			if (ImPlot::BeginPlot("My Plot")) {
				ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
				ImPlot::EndPlot();
			}
			ImGui::End();
			ImGui::End();
		}

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		//size_t upload_size = width * height * 4 * sizeof(char);

		Geom::Texture<Geom::Color4b>::CreateInfo textureCreateInfo{
			(Common::UInt64)width,
			(Common::UInt64)height,
			{ (Geom::Color4b*)pixels, (Common::Size)width * height }
		};

		auto texture = std::make_shared<Geom::Texture<Geom::Color4b>>(textureCreateInfo);

		ImDrawData* draw_data = ImGui::GetDrawData();

		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		// Setup viewport:
		{
			VkViewport viewport;
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = (float)fb_width;
			viewport.height = (float)fb_height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			//vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		}
		float scale[2];
		scale[0] = 2.0f / draw_data->DisplaySize.x;
		scale[1] = 2.0f / draw_data->DisplaySize.y;
		float translate[2];
		translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
		translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];

		glm::mat3 model{ 1.f };
		model[0] = { scale[0], scale[1], 0 };
		model[1] = { translate[0], translate[1], 0 };
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{
					driver_->DrawIndexed(
						model,
						(RAL::Vertex2ftc*)cmd_list->VtxBuffer.Data + pcmd->VtxOffset,
						cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
						cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
						cmd_list->IdxBuffer.Size - pcmd->IdxOffset,
						texture);
				}
			}



		}
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
		RenderImGui();

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