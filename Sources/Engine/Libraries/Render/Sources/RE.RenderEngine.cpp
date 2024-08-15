
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

		RAL::Driver::CreateInfo driverCreateInfo{};
		driverCreateInfo.surface_ = *createInfo.renderSurface_;

		{
			RAL::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "ImGuiVertexShader",
				.type_ = RAL::Shader::Type::Vertex,
				.code_ = createInfo.imguiVertexShader_
			};
			auto vertexShader = api_->CreateShader(vertexShaderCreateInfo);
			RAL::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "ImGuiFragmentShader",
				.type_ = RAL::Shader::Type::Fragment,
				.code_ = createInfo.imguiFragmentShader_
			};
			auto fragmentShader = api_->CreateShader(fragmentShaderCreateInfo);
			driverCreateInfo.imguiPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
				"ImGui",
				vertexShader,
				fragmentShader);
		}

		{
			RAL::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "LinesVertexShader",
				.type_ = RAL::Shader::Type::Vertex,
				.code_ = createInfo.linesVertexShader_
			};
			auto vertexShader = api_->CreateShader(vertexShaderCreateInfo);
			RAL::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "LinesFragmentShader",
				.type_ = RAL::Shader::Type::Fragment,
				.code_ = createInfo.linesFragmentShader_
			};
			auto fragmentShader = api_->CreateShader(fragmentShaderCreateInfo);
			driverCreateInfo.linesPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
				"Lines",
				vertexShader,
				fragmentShader);
		}

		{
			RAL::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "FlatVertexShader",
				.type_ = RAL::Shader::Type::Vertex,
				.code_ = createInfo.vertexShader_
			};
			auto vertexShader = api_->CreateShader(vertexShaderCreateInfo);
			RAL::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "FlatFragmentShader",
				.type_ = RAL::Shader::Type::Fragment,
				.code_ = createInfo.fragmentShader_
			};
			auto fragmentShader = api_->CreateShader(fragmentShaderCreateInfo);
			driverCreateInfo.flatShadedPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
				"Flat",
				vertexShader,
				fragmentShader);
		}

		{
			RAL::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "TexturedVertexShader",
				.type_ = RAL::Shader::Type::Vertex,
				.code_ = createInfo.textureVertexShader_
			};
			auto vertexShader = api_->CreateShader(vertexShaderCreateInfo);
			RAL::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "TexturedFragmentShader",
				.type_ = RAL::Shader::Type::Fragment,
				.code_ = createInfo.textureFragmentShader_
			};
			auto fragmentShader = api_->CreateShader(fragmentShaderCreateInfo);
			driverCreateInfo.texturedPipeline_ = std::make_shared<RAL::Driver::Pipeline>(
				"Textured",
				vertexShader,
				fragmentShader);
		}


		driver_ = api_->CreateDriver(driverCreateInfo);
		driver_->SetCamera(createInfo.camera_);
		driver_->AddLight(createInfo.light_);

		{
			ImGuiIO& io = ImGui::GetIO();
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			std::vector<RAL::Color4b> pixelsRGBA;
			pixelsRGBA.resize(width * height);
			std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));
			RAL::Texture::CreateInfo textureCreateInfo{
				.name_ = "ImGui texture",
				.pixels_ = std::move(pixelsRGBA),
				.size_ = { width, height }
			};

			imguiTextureId_ = driver_->CreateTexture(textureCreateInfo);
		}
	}

	void RenderEngine::RenderImGui() {

		/*if (!ImGui::GetIO().Fonts->IsBuilt()) {
			ImGui::GetIO().Fonts->Build();
		}*/
		/*{
			
			unsigned char* pixels;
			int width, height;
			ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
			size_t upload_size = width * height * 4 * sizeof(char);
		}*/

		ImDrawData* draw_data = ImGui::GetDrawData();
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		//// Setup viewport:
		//{
		//	VkViewport viewport;
		//	viewport.x = 0;
		//	viewport.y = 0;
		//	viewport.width = (float)fb_width;
		//	viewport.height = (float)fb_height;
		//	viewport.minDepth = 0.0f;
		//	viewport.maxDepth = 1.0f;
		//	//vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		//}

		const glm::vec2 scale{
			2.0f / draw_data->DisplaySize.x,
			2.0f / draw_data->DisplaySize.y
		};

		const glm::vec2 translate{
			-1.0f - draw_data->DisplayPos.x * scale[0],
			-1.0f - draw_data->DisplayPos.y * scale[1]
		};


		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{
					driver_->DrawIndexed(
						scale,
						translate,
						(RAL::Vertex2ftc*)cmd_list->VtxBuffer.Data + pcmd->VtxOffset,
						cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
						cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
						cmd_list->IdxBuffer.Size - pcmd->IdxOffset,
						imguiTextureId_);
				}
			}



		}
	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const glm::mat4& position, const Geometry::Model<RAL::Vertex3fnt, RAL::Index16>& model) {

		Model drawnModel;
		/*for (const auto& shape : model) {
			Geometry::VertexCloud<Geometry::Vertex3fnt> verticesColored;
			const auto& vertices = shape.GetVertices();
			for (const auto& vertex : vertices) {
				verticesColored.Add(vertex);
			}
			
			std::vector<RAL::Color4b> pixelsRGBA;
			pixelsRGBA.resize(shape.GetTexture()->GetPixelsNumber());
			std::memcpy(pixelsRGBA.data(), shape.GetTexture()->GetPixels<RAL::Color4b>(), shape.GetTexture()->GetPixelsNumber() * sizeof(RAL::Color4b));

			const RAL::Texture::CreateInfo textureCreateInfo{
				.name_ = "",
				.pixels_ = std::move(pixelsRGBA),
				.size_ = shape.GetTexture()->GetSize()
			};
			const auto textureId = driver_->CreateTexture(textureCreateInfo);

			Common::Index shapeId = driver_->DrawIndexed(
				position,
				(const RAL::Vertex3fnt*)verticesColored.GetData(),
				verticesColored.GetVerticesNumber(),
				shape.GetIndices().GetData(),
				shape.GetIndicesNumber(),
				textureId
			);
			drawnModel.shapes_.push_back({ shapeId });
		}*/

		return drawnModel;
	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const glm::mat4& position, const Geometry::Model<RAL::Vertex3fnc, RAL::Index16>& model) {
		Model drawnModel;
		/*for (const auto& shape : model) {
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
		}*/
		return drawnModel;
	}




	void RenderEngine::SetModelMatrix(const RenderEngine::Model& model, const glm::mat4& modelMatrix) {
		for (auto shape : model.shapes_) {
			driver_->SetModelMatrix(shape.id_, modelMatrix);
		}
	}

	void RenderEngine::Render() {

		//RenderImGui();

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