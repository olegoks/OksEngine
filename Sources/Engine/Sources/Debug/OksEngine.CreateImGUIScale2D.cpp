
#include <Debug/OksEngine.CreateImGUIScale2D.hpp>

#include <Debug/OksEngine.Debug.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

namespace OksEngine {

	void CreateImGUIScale2D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		ImDrawData* draw_data = ImGui::GetDrawData();

		glm::vec2 scale{ 0, 0 };

		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		//int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		//int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		//if (fb_width <= 0 || fb_height <= 0)
		//	return;
		if (draw_data != nullptr) {
			scale.x = 2.0f / draw_data->DisplaySize.x;
			scale.y = 2.0f / draw_data->DisplaySize.y;
		}

		world->CreateComponent<Scale2D>(entityId, scale.x, scale.y);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateImGUIScale2D::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<ImGuiState>()
			.Exclude<Scale2D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateImGUIScale2D::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateImGUIScale2D>().GetId();
	}
}