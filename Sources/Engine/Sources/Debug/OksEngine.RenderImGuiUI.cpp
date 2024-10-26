
#include <Debug/OksEngine.RenderImGuiUI.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Debug/OksEngine.ImGuiState.hpp>
#include <Common/FramesCounter/OksEngine.FramesCounter.hpp>
#include <Common/OksEngine.Counter.hpp>

namespace OksEngine {


	void RenderImGuiUI::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* framesCounter = world->GetComponent<Counter>(secondEntityId);

		ImGuiState* state = world->GetComponent<ImGuiState>(entityId);
		if (framesCounter->value_ % state->fps_ != 0) return;

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		if (!state->driverShapesId_.empty()) {
			for (Common::Id shapeId : state->driverShapesId_) {
				driver->RemoveMeshFromDrawing(shapeId);
			}
			state->driverShapesId_.clear();
		}
		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr) return;
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
					std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

					{
						struct Transform {
							glm::vec2 scale_;
							glm::vec2 translate_;
						};
						Transform transform{ scale,translate };

						RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
							.size_ = sizeof(Transform),
							.type_ = RAL::Driver::UniformBuffer::Type::Const
						};
						RAL::Driver::UniformBuffer::Id transformUBId = driver->CreateUniformBuffer(UBCreateInfo);

						driver->FillUniformBuffer(transformUBId, &transform);

						RAL::Driver::ShaderBinding::Data transformBinding{
							.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
							.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
							.uniformBufferId_ = transformUBId
						};
						shaderBindings.push_back(transformBinding);
					}

					{
						RAL::Driver::ShaderBinding::Data textureBinding{
							.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
							.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
							.textureId_ = state->fontsTextureId_
						};
						shaderBindings.push_back(textureBinding);
					}

					//const Common::Id shapeId = driver->DrawMesh(
					//	"ImGui Pipeline",
					//	(RAL::Vertex2ftc*)cmd_list->VtxBuffer.Data + pcmd->VtxOffset,
					//	cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
					//	RAL::Driver::VertexType::VF2_TF2_CF4,
					//	cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
					//	cmd_list->IdxBuffer.Size - pcmd->IdxOffset,
					//	RAL::Driver::IndexType::UI16,
					//	shaderBindings);

					//state->driverShapesId_.push_back(shapeId);

				}
			}



		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderImGuiUI::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<ImGuiState>(), ECS::Entity::Filter{}.Include<FramesCounter>().Include<Counter>() };
	}

	Common::TypeId RenderImGuiUI::GetTypeId() const noexcept {
		return Common::TypeInfo<RenderImGuiUI>().GetId();
	}

}