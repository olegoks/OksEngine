
#include <Debug/OksEngine.DebugUI.System.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

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

				bool isOpen = true;
				ImGui::Begin("ECS Inspector", &isOpen, 0);

				ImGui::End();

				ImGui::Begin("ECS World Info", &isOpen, 0);

				ImGui::TextDisabled("Entities number: %d", world->GetEntitiesNumber());
				ImGui::TextDisabled("Entities:");
				for (ECS::Entity::Id id : world->GetEntitiesId()) {
					ImGui::TextDisabled("Id: %d", id);
					if (world->IsComponentExist<Position>(id)) {
						Position* position = world->GetComponent<Position>(id);
						ImGui::TextDisabled("Position: %f, %f, %f", position->GetX(), position->GetY(), position->GetZ());
					}
				}


				ImGui::End();
			}
		}

	}

	Common::TypeId ECSInspectorSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<ECSInspectorSystem>().GetId();
	}
}