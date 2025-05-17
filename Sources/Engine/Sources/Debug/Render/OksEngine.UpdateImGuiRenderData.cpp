#include <Debug/Render\auto_OksEngine.UpdateImGuiRenderData.hpp>

#include <Geometry.IndexBuffer.hpp>

namespace OksEngine {
	void UpdateImGuiRenderData::Update(
		ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
		ImGuiDriverVertexBuffer* imGuiDriverVertexBuffer,
		ImGuiDriverIndexBuffer* imGuiDriverIndexBuffer,
		ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr) {
			return;
		}
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;


		//const glm::vec2 scale{
		//	2.0f / draw_data->DisplaySize.x,
		//	2.0f / draw_data->DisplaySize.y
		//};

		//const glm::vec2 translate{
		//	-1.0f - draw_data->DisplayPos.x * scale[0],
		//	-1.0f - draw_data->DisplayPos.y * scale[1]
		//};

		//Geom::VertexCloud<Geom::Vertex2f> vertices;
		//DS::Vector<Geom::UV2f> uvs;
		//DS::Vector<Geom::Color4b> colors;
		Geom::IndexBuffer indices;

		Geom::VertexCloud<RAL::Vertex2ftc> vertices2ftc;

		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{

					const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;

					Geom::IndexBuffer imguiCmdIndices;
					imguiCmdIndices.Add(cmd_list->IdxBuffer.Data + pcmd->IdxOffset, cmd_list->IdxBuffer.Size - pcmd->IdxOffset);

					indices.AddNextMesh(vertices2ftc.GetVerticesNumber(), imguiCmdIndices);

					const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;
					for (Common::Index i = 0; i < verticesNumber; i++) {
						const ImDrawVert* imVertex = imVertices + i;
						const ImU32 color = imVertex->col;
						vertices2ftc.Add(
							RAL::Vertex2ftc{
								Geom::Vertex2f{ imVertex->pos.x, imVertex->pos.y },
								Geom::UV2f{ imVertex->uv.x, imVertex->uv.y },
								Geom::Color4b{ (Common::Byte*)&color }
							}
						);
						//	});
						//vertices.Add(Geom::Vertex2f{ imVertex->pos.x, imVertex->pos.y });
						//uvs.PushBack(Geom::UV2f{ imVertex->uv.x, imVertex->uv.y });
						//
						//colors.PushBack(Geom::Color4b{ (Common::Byte*)&color }
						// Geom::Color4f{
							//((Common::Byte*)&color)[0] / 255.f,
							//((Common::Byte*)&color)[1] / 255.f,
							//((Common::Byte*)&color)[2] / 255.f,
							//((Common::Byte*)&color)[3] / 255.f }*/);
					}

				}
			}
		}
		if (vertices2ftc.GetVerticesNumber() == 0) {
			return;
		}

		auto driver = renderDriver->driver_;

		//Resize buffers if need.
		if (vertices2ftc.GetSizeInBytes() > renderDriver->driver_->GetVBSizeInBytes(imGuiDriverVertexBuffer->id_)) {
			renderDriver->driver_->ResizeVertexBuffer(
				imGuiDriverVertexBuffer->id_,
				vertices2ftc.GetSizeInBytes());
			imGuiDriverVertexBuffer->capacity_ = vertices2ftc.GetSizeInBytes();
		}
		if (indices.GetSizeInBytes() > renderDriver->driver_->GetIBSizeInBytes(imGuiDriverIndexBuffer->id_)) {
			renderDriver->driver_->ResizeIndexBuffer(
				imGuiDriverIndexBuffer->id_,
				indices.GetSizeInBytes());

			imGuiDriverIndexBuffer->capacity_ = vertices2ftc.GetSizeInBytes();
		}

		//Fill data.
		renderDriver->driver_->FillVertexBuffer(
			imGuiDriverVertexBuffer->id_,
			0,
			vertices2ftc.GetData(), 
			vertices2ftc.GetVerticesNumber());
		imGuiDriverVertexBuffer->size_ = vertices2ftc.GetSizeInBytes();

		driver->FillIndexBuffer(
			imGuiDriverIndexBuffer->id_,
			0,
			indices.GetData(),
			indices.GetIndicesNumber());

		imGuiDriverIndexBuffer->size_ = indices.GetSizeInBytes();

	};
}