
#include <Debug/OksEngine.CreateImGUIPosition2D.hpp>

#include <Debug/OksEngine.Debug.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

namespace OksEngine {

	void CreateImGUIPosition2D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		ImDrawData* draw_data = ImGui::GetDrawData();

		glm::vec2 translate{ 0, 0 };
		
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		//int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		//int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		//if (fb_width <= 0 || fb_height <= 0)
		//	return;
		if (draw_data != nullptr) {
			const glm::vec2 scale{
				2.0f / draw_data->DisplaySize.x,
				2.0f / draw_data->DisplaySize.y
			};
			translate.x = -1.0f - draw_data->DisplayPos.x * scale[0];
			translate.y = -1.0f - draw_data->DisplayPos.y * scale[1];
		}

		world->CreateComponent<Position2D>(entityId, translate.x, translate.y);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateImGUIPosition2D::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<ImGuiState>()
			.Exclude<Position2D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateImGUIPosition2D::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateImGUIPosition2D>().GetId();
	}
}