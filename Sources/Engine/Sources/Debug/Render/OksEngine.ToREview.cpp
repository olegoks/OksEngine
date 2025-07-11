
//#include <Debug/Render/auto_OksEngine.CreateImGUIRenderData.hpp>

//#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	/*void CreateImGuiRenderData::Update(
		ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
		const ModelEntity* modelEntity, ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {*/

		//auto driver = renderDriver->driver_;
		//ImDrawData* draw_data = ImGui::GetDrawData();
		//if (draw_data == nullptr) return;
		//// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		//int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		//int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		//if (fb_width <= 0 || fb_height <= 0)
		//	return;

		//const glm::vec2 scale{
		//	2.0f / draw_data->DisplaySize.x,
		//	2.0f / draw_data->DisplaySize.y
		//};

		//const glm::vec2 translate{
		//	-1.0f - draw_data->DisplayPos.x * scale[0],
		//	-1.0f - draw_data->DisplayPos.y * scale[1]
		//};
		//struct Transform {
		//	glm::vec2 scale_;
		//	glm::vec2 translate_;
		//};
		//Transform transform{ scale,translate };

		//Geom::VertexCloud<Geom::Vertex2f> vertices;
		//DS::Vector<Geom::UV2f> uvs;
		//DS::Vector<Geom::Color4b> colors;
		//Geom::IndexBuffer indices;

		//Common::Size commonVerticesNumber = 0;
		//Common::Size commonIndicesNumber = 0;

		//for (int n = 0; n < draw_data->CmdListsCount; n++) {
		//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
		//	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		//	{
		//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
		//		{
		//			commonVerticesNumber += cmd_list->VtxBuffer.Size - pcmd->VtxOffset;
		//			commonIndicesNumber += cmd_list->IdxBuffer.Size - pcmd->IdxOffset;
		//		}
		//	}
		//}

		//vertices.Reserve(commonVerticesNumber);
		//uvs.Reserve(commonVerticesNumber);
		//colors.Reserve(commonVerticesNumber);
		//indices.Reserve(commonIndicesNumber);

		//for (int n = 0; n < draw_data->CmdListsCount; n++) {
		//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
		//	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		//	{
		//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
		//		{

		//			const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;


		//			Geom::IndexBuffer imguiCmdIndices;
		//			imguiCmdIndices.Add(cmd_list->IdxBuffer.Data + pcmd->IdxOffset, cmd_list->IdxBuffer.Size - pcmd->IdxOffset);

		//			indices.AddNextMesh(vertices.GetVerticesNumber(), imguiCmdIndices);

		//			const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;
		//			for (Common::Index i = 0; i < verticesNumber; i++) {
		//				const ImDrawVert* imVertex = imVertices + i;
		//				vertices.Add(Geom::Vertex2f{ imVertex->pos.x, imVertex->pos.y });
		//				uvs.PushBack(Geom::UV2f{ imVertex->uv.x, imVertex->uv.y } );
		//				const ImU32 color = imVertex->col;
		//				colors.PushBack(Geom::Color4b{ (Common::Byte*) & color});
		//				//colors.PushBack(Geom::Color4f{ 
		//				//	((Common::Byte*)&color)[0] / 255.f,
		//				//	((Common::Byte*)&color)[1] / 255.f, //0.f,//
		//				//	((Common::Byte*)&color)[2] / 255.f,//0.f,//
		//				//	((Common::Byte*)&color)[3] / 255.f
		//				//	});
		//			}

		//		}
		//	}
		//}


		//auto* modelEntity = world->GetComponent<ModelEntity>(entityId);
		//auto* meshEntities = world->GetComponent<ChildEntities>(modelEntity->id_);

		//OS::Assert(meshEntities->entitiesIds_.size() == 1);

		//const ECS::Entity::Id meshEntityId = meshEntities->entitiesIds_[0];

		//auto* meshVertices = world->GetComponent<Vertices2D>(meshEntityId);
		//meshVertices->vertices_ = std::move(vertices);

		//auto* meshIndices = world->GetComponent<Indices>(meshEntityId);
		//meshIndices->indices_ = std::move(indices);

		//auto* meshUvs = world->GetComponent<UVs>(meshEntityId);
		//meshUvs->uvs_ = std::move(uvs);

		//auto* meshColors = world->GetComponent<Colors>(meshEntityId); // NEED TO CHANGE COLOR TO COLOR4F, or... No
		//meshColors->colors_ = std::move(colors);
	//}

}