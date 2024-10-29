
#include <Debug/Render/OksEngine.CreateImGUIRenderData.hpp>

#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>
#include <Debug/OksEngine.Debug.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateImGUIRenderData::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
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
		//DS::Vector<Geom::Color4f> colors;
		//Geom::IndexBuffer indices;

		//for (int n = 0; n < draw_data->CmdListsCount; n++) {
		//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
		//	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		//	{
		//		const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
		//		{

		//			const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;

		//			const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;
		//			for (Common::Index i = 0; i < verticesNumber; i++) {
		//				const ImDrawVert* imVertex = imVertices + i;
		//				vertices.Add(Geom::Vertex2f{ imVertex->pos.x, imVertex->pos.y });
		//				uvs.PushBack(Geom::UV2f{ imVertex->uv.x, imVertex->uv.y } );
		//				const ImU32 color = imVertex->col;
		//				colors.PushBack(Geom::Color4f{ 
		//					((Common::Byte*)&color)[0] / 255.f,
		//					((Common::Byte*)&color)[1] / 255.f, 
		//					((Common::Byte*)&color)[2] / 255.f,
		//					((Common::Byte*)&color)[3] / 255.f});
		//			}

		//		}
		//	}
		//}
		//auto* modelEntity = world->GetComponent<ModelEntity>(entityId);
		//auto* meshEntityIds = world->GetComponent<ChildEntities>(modelEntity->id_);

		//OS::Assert(meshEntityIds->entitiesIds_.size() == 1);

		//const ECS::Entity::Id meshEntityId = meshEntityIds->entitiesIds_[0];

		//world->CreateComponent<Vertices2D>(meshEntityId, vertices);
		//world->CreateComponent<Indices>(meshEntityId, indices);
		//world->CreateComponent<UVs>(meshEntityId, uvs);
		//world->CreateComponent<Colors>(meshEntityId, colors); // NEED TO CHANGE COLOR TO COLOR4F, or... No

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateImGUIRenderData::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}.Include<ImGuiState>().Include<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	  
	Common::TypeId CreateImGUIRenderData::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateImGUIRenderData>().GetId();
	}
}