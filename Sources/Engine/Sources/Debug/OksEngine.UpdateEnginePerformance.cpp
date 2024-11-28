
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

#include <Debug/OksEngine.UpdateEnginePerformance.hpp>

#include <Debug/OksEngine.EnginePerformance.hpp>
#include <Common/FramesCounter/OksEngine.FramesCounter.hpp>
#include <Common/OksEngine.Counter.hpp>

namespace OksEngine {

	UpdateEnginePerformance::UpdateEnginePerformance(Context& context) : ECSSystem{ context } { }

	void UpdateEnginePerformance::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* enginePerformance = world->GetComponent<EnginePerformance>(entityId);

		if (enginePerformance->show_) {

			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem(
						"Performance profiler",
						nullptr,
						&enginePerformance->show_);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}


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



			auto* framesCounter = world->GetComponent<Counter>(secondEntityId);
			ImGui::TextDisabled("Frame: %d", framesCounter->value_);
			//ImGui::Begin("My Window");
			if (ImPlot::BeginPlot("My Plot")) {
				ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
				ImPlot::EndPlot();
			}
			//ImGui::End();
			ImGui::End();
		}


	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateEnginePerformance::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<EnginePerformance>().Include<FramesCounter>(), ECS::Entity::Filter{}.Include<FramesCounter>().Include<Counter>() };
		return filter;
	}

	Common::TypeId UpdateEnginePerformance::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateEnginePerformance>().GetId();
	}

}