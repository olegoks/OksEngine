
#include <Debug/OksEngine.DebugUI.System.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Common/OksEngine.MapRigidBodyToRenderGeometry.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>


#include <implot.h>
#include <implot_internal.h>

namespace OksEngine {


	void MainMenuBarSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* menuBar = world->GetComponent<MainMenuBar>(entityId);
		if (menuBar != nullptr) {
			//GetContext().GetRenderSubsystem()->AddImGuiCallback([]() {


				//if (ImGui::BeginMainMenuBar()) {
				//	// Add items to the menu bar.
				//	if (ImGui::BeginMenu("Engine")) {
				//		static bool showPerformanceProfiler_ = false;
				//		ImGui::MenuItem("Performance profiler", nullptr, &menuBar->showPerformanceProfiler_);
				//		ImGui::MenuItem("ECS Inspector", NULL, false, false);
				//		ImGui::EndMenu();
				//	}
				//	// End the menu bar.
				//	ImGui::EndMainMenuBar();
				//}
				//if (ImGui::BeginMainMenuBar()) {
				//	// Add items to the menu bar.
				//	if (ImGui::BeginMenu("Engine")) {
				//		ImGui::MenuItem("Render", NULL, false, false);
				//		ImGui::MenuItem("Help", NULL, false, false);
				//		ImGui::EndMenu();
				//	}
				//	// End the menu bar.
				//	ImGui::EndMainMenuBar();
				//}
				//ImGui::ShowDemoWindow();
				//});
		}
	}

	void EnginePerformanceSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* enginePerformance = world->GetComponent<EnginePerformance>(entityId);
		if (enginePerformance != nullptr) {
			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem("Performance profiler", nullptr, &enginePerformance->show_);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}
			if (enginePerformance->show_) {

				bool isOpen = true;
				ImGui::Begin("Engine performance", &isOpen, 0);


				static Common::UInt64 renderCalls = 0;
				++renderCalls;

				using namespace std::chrono_literals;
				std::chrono::high_resolution_clock::time_point now;
				static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

				now = std::chrono::high_resolution_clock::now();
				auto delta = now - point;

				auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
				static Common::Size lastFps = 0;
				static std::vector<Common::Size> fps_;
				static std::vector<Common::Size> timePoints_;
				if (microseconds > 1000000) {
					Common::Size framesPerSecond = renderCalls * 1000000 / microseconds;
					ImGui::TextDisabled("Fps: %d", framesPerSecond);
					fps_.push_back(framesPerSecond);
					timePoints_.push_back(fps_.size());
					renderCalls = 0;
					point = now;
					lastFps = framesPerSecond;

				}
				else {
					ImGui::TextDisabled("Fps: %d", lastFps);
				}

				//ImGui::Begin("My Window");
				if (ImPlot::BeginPlot("My Plot")) {
					ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
					ImPlot::EndPlot();
				}
				//ImGui::End();
				ImGui::End();
			}
		}



	}


	ECSInspectorSystem::ECSInspectorSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void ECSInspectorSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* ecsInspector = world->GetComponent<ECSInspector>(entityId);
		if (ecsInspector != nullptr) {

			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem("ECS Inspector", nullptr, &ecsInspector->show_);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}
			if (ecsInspector->show_) {

				/*bool isOpen = true;
				ImGui::Begin("ECS Inspector", &isOpen, 0);

				ImGui::End();*/
				bool isOpen = true;
				ImGui::Begin("ECS World Info", &isOpen, 0);

				ImGui::TextDisabled("Entities number: %d", world->GetEntitiesNumber());
				ImGui::TextDisabled("Entities:");
				if (ImGui::Button("Add entity")) {
					ECS::Entity::Id id = world->CreateEntity();
				}
				ImGui::Separator();
				//ImGui::SameLine();
				for (ECS::Entity::Id id : world->GetEntitiesId()) {

					ImGui::TextDisabled("Id: %d", id);
					if (world->IsComponentExist<Position>(id)) {
						std::string name = "Position##" + std::to_string(id);
						if (ImGui::CollapsingHeader(name.c_str())) {
							Position* position = world->GetComponent<Position>(id);
							ImGui::InputFloat("X", &position->translate_.x/*, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_AlwaysOverwrite*/);// ImGui::SameLine();
							ImGui::InputFloat("Y", &position->translate_.y);// ImGui::SameLine();
							ImGui::InputFloat("Z", &position->translate_.z);// ImGui::SameLine();
							ImGui::Spacing();
						}
					}

					{
						bool isExist = world->IsComponentExist<ImmutableRenderGeometry>(id);
						if (ImGui::CollapsingHeader("ImmutableRenderGeometry", &isExist)) {
							ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(id);
							ImGui::TextDisabled("Immutable render geometry: %s", renderGeometry->modelObjFileName_.c_str());
						}
						if (!isExist) {
							if (world->IsComponentExist<ImmutableRenderGeometry>(id)) {
								world->RemoveComponent<ImmutableRenderGeometry>(id);
							}
						}
					}
					if (world->IsComponentExist<StaticRigidBodyCustomMeshShape>(id)) {
						if (ImGui::CollapsingHeader("StaticRigidBodyCustomMeshShape")) {
							StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape = world->GetComponent<StaticRigidBodyCustomMeshShape>(id);
							ImGui::TextDisabled("StaticRigidBodyCustomMeshShape: %s", staticRigidBodyCustomMeshShape->geomName_);
							if (ImGui::Button("Delete")) { world->RemoveComponent<StaticRigidBodyCustomMeshShape>(id); }
							ImGui::Spacing();
						}
					}
					if (world->IsComponentExist<MapRigidBodyToRenderGeometry>(id)) {
						if (ImGui::CollapsingHeader("MapRigidBodyToRenderGeometry")) {
							MapRigidBodyToRenderGeometry* mapRigidBodyToRenderGeometry = world->GetComponent<MapRigidBodyToRenderGeometry>(id);
							ImGui::TextDisabled("MapRigidBodyToRenderGeometry");
							if (ImGui::Button("Delete")) { world->RemoveComponent<MapRigidBodyToRenderGeometry>(id); }
							ImGui::Spacing();
						}
					}

					auto& state = ecsInspector->GetCreateState(id);
					ImGui::SeparatorText("Add component");
					const char* items[] = { "Position", "ImmutableRenderGeometry", "StaticRigidBodyCustomMeshShape", "MapRigidBodyToRenderGeometry" };
					std::string name = "Add component" + std::to_string(id);
					ImGui::Combo(name.c_str(), &state.currentAddComponentIndex_, items, 4);
					ImGui::PushID(name.c_str());
					if (ImGui::Button("Add component")) {
						const std::string currentComponent = items[state.currentAddComponentIndex_];
						if (currentComponent == "Position") {
							if (!world->IsComponentExist<Position>(id)) {
								world->CreateComponent<Position>(id);
							}
						}
						else if (currentComponent == "ImmutableRenderGeometry") {
							if (!world->IsComponentExist<ImmutableRenderGeometry>(id)) {
								world->CreateComponent<ImmutableRenderGeometry>(id);
							}
						}
					}
					ImGui::PopID();
					ImGui::Separator();


				}
				ImGui::End();
				ImGui::ShowDemoWindow();

			}
		}

	}

	Common::TypeId ECSInspectorSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<ECSInspectorSystem>().GetId();
	}
}