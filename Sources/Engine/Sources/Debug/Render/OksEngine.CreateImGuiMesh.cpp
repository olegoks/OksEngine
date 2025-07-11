
//#include <Debug/Render/auto_OksEngine.CreateImGUIModel.hpp>

//#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	//void CreateImGuiModel::Update(
	//	ECS2::Entity::Id entityId,
	//	const ImGuiState* imGuiState) {

	//	ImDrawData* draw_data = ImGui::GetDrawData();
	//	if (draw_data == nullptr) return;
	//	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	//	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	//	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	//	if (fb_width <= 0 || fb_height <= 0)
	//		return;


	//	const glm::vec2 scale{
	//		2.0f / draw_data->DisplaySize.x,
	//		2.0f / draw_data->DisplaySize.y
	//	};

	//	const glm::vec2 translate{
	//		-1.0f - draw_data->DisplayPos.x * scale[0],
	//		-1.0f - draw_data->DisplayPos.y * scale[1]
	//	};

	//	Geom::VertexCloud<Geom::Vertex2f> vertices;
	//	DS::Vector<Geom::UV2f> uvs;
	//	DS::Vector<Geom::Color4b> colors;
	//	Geom::IndexBuffer indices;

	//	for (int n = 0; n < draw_data->CmdListsCount; n++) {
	//		const ImDrawList* cmd_list = draw_data->CmdLists[n];
	//		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
	//		{
	//			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
	//			{

	//				const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;

	//				Geom::IndexBuffer imguiCmdIndices;
	//				imguiCmdIndices.Add(cmd_list->IdxBuffer.Data + pcmd->IdxOffset, cmd_list->IdxBuffer.Size - pcmd->IdxOffset);

	//				indices.AddNextMesh(vertices.GetVerticesNumber(), imguiCmdIndices);

	//				const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;
	//				for (Common::Index i = 0; i < verticesNumber; i++) {
	//					const ImDrawVert* imVertex = imVertices + i;
	//					vertices.Add(Geom::Vertex2f{ imVertex->pos.x, imVertex->pos.y });
	//					uvs.PushBack(Geom::UV2f{ imVertex->uv.x, imVertex->uv.y });
	//					const ImU32 color = imVertex->col;
	//					colors.PushBack(Geom::Color4b{ (Common::Byte*)&color }/*Geom::Color4f{
	//						((Common::Byte*)&color)[0] / 255.f,
	//						((Common::Byte*)&color)[1] / 255.f,
	//						((Common::Byte*)&color)[2] / 255.f,
	//						((Common::Byte*)&color)[3] / 255.f }*/);
	//				}


	//			}
	//		}
	//	}
	//	if (vertices.GetVerticesNumber() == 0) {
	//		return;
	//	}
	//	ECS2::Entity::Id modelEntityId = CreateEntity();
	//	CreateComponent<Position2D>(modelEntityId, translate.x, translate.y);
	//	CreateComponent<Scale2D>(modelEntityId, scale.x, scale.y);
	//	CreateComponent<Rotation2D>(modelEntityId, 1, 0, 0, 0);
	//	CreateComponent<Vertices2D>(modelEntityId, vertices);
	//	CreateComponent<Indices>(modelEntityId, indices);
	//	CreateComponent<UVs>(modelEntityId, uvs);
	//	CreateComponent<Colors>(modelEntityId, colors); // NEED TO CHANGE COLOR TO COLOR4F, or... No

	//	// ImGuiIO& io = ImGui::GetIO();
	//	// unsigned char* pixels;
	//	// int width, height;
	//	// io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	//	// std::vector<RAL::Color4b> pixelsRGBA;
	//	// pixelsRGBA.resize(width * height);
	//	// std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));

	//	// Geom::Texture texture = Geom::CreateTexture(pixelsRGBA.data(), width, height);
	//	// CreateComponent<TextureInfo>(modelEntityId, "ImGUI texture");
	//	// CreateComponent<Texture>(modelEntityId, texture.GetWidth(), texture.GetHeight(), texture.GetPixelsArray());

	//	//CreateComponent<ChildEntities>(modelEntityId, std::vector{ meshEntityId });

	//	// CreateComponent<ModelEntity>(entityId, modelEntityId);

	//}

}