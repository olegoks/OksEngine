
#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <Debug/Performance/auto_OksEngine.DrawEnginePerformance.hpp>

namespace OksEngine {

	void DrawEnginePerformance::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0,
		const EnginePerformance* enginePerformance0) {

		// if (enginePerformance->show_) {


		// 	bool isOpen = true;
		// 	ImGui::Begin("Engine performance", &isOpen, 0);


		// 	static Common::UInt64 renderCalls = 0;
		// 	++renderCalls;

		// 	using namespace std::chrono_literals;
		// 	std::chrono::high_resolution_clock::time_point now;
		// 	static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

		// 	now = std::chrono::high_resolution_clock::now();
		// 	auto delta = now - point;

		// 	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
		// 	static Common::Size lastFps = 0;
		// 	static std::vector<Common::Size> fps_;
		// 	static std::vector<Common::Size> timePoints_;
		// 	if (microseconds > 1000000) {
		// 		Common::Size framesPerSecond = renderCalls * 1000000 / microseconds;
		// 		ImGui::TextDisabled("Fps: %d", framesPerSecond);
		// 		fps_.push_back(framesPerSecond);
		// 		timePoints_.push_back(fps_.size());
		// 		renderCalls = 0;
		// 		point = now;
		// 		lastFps = framesPerSecond;

		// 	}
		// 	else {
		// 		ImGui::TextDisabled("Fps: %d", lastFps);
		// 	}

		// 	ImGui::TextDisabled("Frame: %d", framesCounter->value_);
		// 	//ImGui::Begin("My Window");
		// 	if (ImPlot::BeginPlot("My Plot")) {
		// 		ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
		// 		ImPlot::EndPlot();
		// 	}

		// 	ImGui::End();
		// }


	}

}