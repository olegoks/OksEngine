
#include <Debug/auto_OksEngine.CreateImGUIScale2D.hpp>

#include <glm/vec2.hpp>

namespace OksEngine {

	void CreateImGUIScale2D::Update(
		ECS2::Entity::Id entityId,
		const ImGuiState* imGuiState) {
		//ImDrawData* draw_data = ImGui::GetDrawData();

		//glm::vec2 scale{ 0, 0 };

		//// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		////int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		////int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		////if (fb_width <= 0 || fb_height <= 0)
		////	return;
		//if (draw_data != nullptr) {
		//	scale.x = 2.0f / draw_data->DisplaySize.x;
		//	scale.y = 2.0f / draw_data->DisplaySize.y;
		//}

		//world->CreateComponent<Scale2D>(entityId, scale.x, scale.y);

	}

}