#include <Debug/Render\auto_OksEngine.UpdateImGuiRenderData.hpp>

#include <Common.StringLiterals.hpp>
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

		static Geom::IndexBuffer<Common::UInt32> indices;

		using namespace Common;
		indices.Reserve(10_MB / indices.GetIndexSize());
		indices.Clear();

		static Geom::VertexCloud<RAL::Vertex2ftc> vertices2ftc;

		vertices2ftc.Reserve(30_MB / vertices2ftc.GetVertexSize());
		vertices2ftc.Clear();

		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{

					const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;

					indices.AddNextMesh(
						vertices2ftc.GetVerticesNumber(),
						cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
						cmd_list->IdxBuffer.Size - pcmd->IdxOffset);

					const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;

					vertices2ftc.Add((RAL::Vertex2ftc*)imVertices, verticesNumber);

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