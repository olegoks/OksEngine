
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <Debug/auto_OksEngine.UpdateMainMenuBar.hpp>

namespace OksEngine {


	//void UpdateMainMenuBar::BeforeUpdate(ECS::World* world) {
	//	ImGuiViewport* viewport = (ImGuiViewport*)ImGui::GetMainViewport();
	//	ImGuiID mainDockSpaceId = ImGui::DockSpaceOverViewport(viewport->ID, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
	//}

	void UpdateMainMenuBar::Update(
		ECS2::Entity::Id entityId, MainMenuBar* mainMenuBar,
		ImGuiState* imGuiState) {

	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateMainMenuBar::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<MainMenuBar>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}