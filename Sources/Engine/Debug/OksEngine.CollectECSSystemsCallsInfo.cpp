#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>

#include "OksEngine.CollectECSSystemsCallsInfo.hpp"

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
					if (ImGui::CollapsingHeader(systemCallInfo.name_.c_str())) {}
				}
				ImGui::Unindent(20.0f);
			}
		}
		
		ImGui::End();

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CollectECSSystemsCallsInfo::GetFilter() const noexcept
	{
		return { ECS::Entity::Filter{}, ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CollectECSSystemsCallsInfo::GetTypeId() const noexcept
	{
		return Common::TypeInfo<CollectECSSystemsCallsInfo>().GetId();
	}

}