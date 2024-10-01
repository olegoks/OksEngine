
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <Debug/OksEngine.UpdateMainMenuBar.hpp>

#include <Debug/OksEngine.MainMenuBar.hpp>

namespace OksEngine {

	UpdateMainMenuBar::UpdateMainMenuBar(Context& context) : ECSSystem{ context } { }

	void UpdateMainMenuBar::BeforeUpdate(ECS::World* world) {
		ImGuiViewport* viewport = (ImGuiViewport*)ImGui::GetMainViewport();
		ImGuiID mainDockSpaceId = ImGui::DockSpaceOverViewport(viewport->ID, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
	}

	void UpdateMainMenuBar::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateMainMenuBar::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<MainMenuBar>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId UpdateMainMenuBar::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateMainMenuBar>().GetId();
	}

}