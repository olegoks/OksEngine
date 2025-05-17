#include <Debug/Render/Transform\auto_OksEngine.UpdateImGuiDriverTransform2D.hpp>

#include <Debug/Render/Transform/OksEngine.ImGuiTransform.hpp>

namespace OksEngine {
void UpdateImGuiDriverTransform2D::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const DriverTransform2D* driverTransform2D, ECS2::Entity::Id entity2Id,
    RenderDriver* renderDriver) {

	ImDrawData* draw_data = ImGui::GetDrawData();
	if (draw_data == nullptr) return;
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;


	const glm::vec2 scale{
		2.0f / draw_data->DisplaySize.x,
		2.0f / draw_data->DisplaySize.y
	};

	const glm::vec2 translate{
		-1.0f - draw_data->DisplayPos.x * scale[0],
		-1.0f - draw_data->DisplayPos.y * scale[1]
	};

	ImGuiTransform transform{
		scale,
		translate
	};

	auto driver = renderDriver->driver_;

	driver->FillUniformBuffer(driverTransform2D->id_, &transform);

};
}