
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <Debug/MainMenu/auto_OksEngine.DrawMainMenuBar.hpp>

namespace OksEngine {


	//void UpdateMainMenuBar::BeforeUpdate(ECS::World* world) {
	//	ImGuiViewport* viewport = (ImGuiViewport*)ImGui::GetMainViewport();
	//	ImGuiID mainDockSpaceId = ImGui::DockSpaceOverViewport(viewport->ID, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
	//}

	void DrawMainMenuBar::Update(
		ECS2::Entity::Id entity0id, const MainMenuBar* mainMenuBar0,
		const ImGuiState* imGuiState0) {

		if (ImGui::BeginMainMenuBar()) {
			// Add items to the menu bar.
			if (ImGui::BeginMenu("Engine")) {
				if (ImGui::MenuItem(
					"Performance profiler",
					nullptr)) {
					CreateComponent<EnginePerformance>(entity0id);
				}
				ImGui::EndMenu();
			}
			// End the menu bar.
			ImGui::EndMainMenuBar();
		}

	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateMainMenuBar::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<MainMenuBar>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}