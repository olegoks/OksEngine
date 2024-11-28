#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>

#include "OksEngine.CollectECSSystemsCallsInfo.hpp"

#include <Debug/OksEngine.ImGuiState.hpp>

namespace OksEngine {

	void CollectECSSystemsCallsInfo::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		//if (ImGui::BeginMainMenuBar()) {
		//	// Add items to the menu bar.
		//	if (ImGui::BeginMenu("Engine")) {
		//		ImGui::MenuItem(
		//			"Performance profiler",
		//			nullptr,
		//			&enginePerformance->show_);
		//		ImGui::EndMenu();
		//	}
		//	// End the menu bar.
		//	ImGui::EndMainMenuBar();
		//}



		bool isOpen = true;
		ImGui::Begin("Systems", &isOpen, 0);



		for (const ECS::World::DebugInfo::FrameInfo& frameInfo : world->debugInfo_.framesInfos_) {
			if (ImGui::CollapsingHeader((std::string{ "Frame " } + std::to_string(frameInfo.index_)).c_str())) {
				ImGui::Indent(20.0f);
				for (const ECS::World::DebugInfo::FrameInfo::SystemCallInfo systemCallInfo : frameInfo.systemsCallsInfos_) {

					if (systemCallInfo.secondEntityId_.IsInvalid()) {
						if (ImGui::CollapsingHeader((systemCallInfo.name_ + " EntityId: " + std::to_string(systemCallInfo.firstEntityId_)).c_str())) {
							for (auto& component : systemCallInfo.componentsNames_) {
								ImGui::TextDisabled(component.c_str());
							}
						}
					}
					else {
						if (ImGui::CollapsingHeader((systemCallInfo.name_ + " First EntityId: " + std::to_string(systemCallInfo.firstEntityId_) + " Second EntityId: " + std::to_string(systemCallInfo.secondEntityId_)).c_str())) {
							for (auto& component : systemCallInfo.componentsNames_) {
								ImGui::TextDisabled(component.c_str());
							}
						
						}
					}
				}
				ImGui::Unindent(20.0f);
			}
		}
		
		ImGui::End();

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CollectECSSystemsCallsInfo::GetFilter() const noexcept
	{
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<ImGuiState>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CollectECSSystemsCallsInfo::GetTypeId() const noexcept
	{
		return Common::TypeInfo<CollectECSSystemsCallsInfo>().GetId();
	}

}