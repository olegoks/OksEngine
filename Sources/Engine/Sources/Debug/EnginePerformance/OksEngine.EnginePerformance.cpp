#pragma once

#include <Debug\EnginePerformance\auto_OksEngine.EnginePerformance.hpp>

#include <implot/implot.h>

namespace OksEngine
{

	void CreateEnginePerformanceMenuItem::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImPlotState* imPlotState0,
		const MainMenuBar* mainMenuBar0) {

		CreateComponent<EnginePerformanceMenuItem>(entity0id);

	}

	void ProcessEnginePerformanceMenuItem::Update(
		ECS2::Entity::Id entity0id, const MainMenuBar* mainMenuBar0, const ImGuiState* imGuiState0,
		const ImPlotState* imPlotState0, const ECSMenu* eCSMenu0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0) {


		if (mainMenuBar0->show_) {
			if (ImGui::MenuItem("EnginePerformance")) {
				if (!IsComponentExist<EnginePerformanceWindow>(entity0id)) {
					CreateComponent<EnginePerformanceWindow>(entity0id);
				}
			}
		}

	}

	void CreateEnginePerformance::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const ImPlotState* imPlotState0,
		const MainMenuBar* mainMenuBar0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0,
		const EnginePerformanceWindow* enginePerformanceWindow0) {

		CreateComponent<EnginePerformance>(entity0id);

		CreateComponent<FPSPlotData>(entity0id);

	}


	void BeginEnginePerformanceWindow::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImPlotState* imPlotState0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0,
		const EnginePerformanceWindow* enginePerformanceWindow0, const EnginePerformance* enginePerformance0) {

		bool isOpen = true;

		ImGui::Begin("Engine performance",
			&isOpen,
			ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (!isOpen) {
			RemoveComponent<EnginePerformanceWindow>(entity0id);
		}
	}

	void UpdateFPSPlotData::Update(
		ECS2::Entity::Id entity0id,
		const FramesCounter* framesCounter0,

		ECS2::Entity::Id entity1id,
		const TimeSinceEngineStart* timeSinceEngineStart1,

		ECS2::Entity::Id entity2id,
		FPSPlotData* fPSPlotData2) {

		fPSPlotData2->microsecondsFromEngineStart_.push_back(timeSinceEngineStart1->microseconds_);

		if (fPSPlotData2->microsecondsFromEngineStart_.size() > 100) {
			fPSPlotData2->microsecondsFromEngineStart_.clear();
		}
	}


	void ShowCurrentFPS::Update(
		ECS2::Entity::Id entity0id, 
		const ImGuiState* imGuiState0, 
		const ImPlotState* imPlotState0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0,
		const EnginePerformanceWindow* enginePerformanceWindow0, 
		const EnginePerformance* enginePerformance0,
		const FPSPlotData* fPSPlotData0,
		
		ECS2::Entity::Id entity1id,
		const FramesCounter* framesCounter1,

		ECS2::Entity::Id entity2id,
		const TimeSinceEngineStart* timeSinceEngineStart2) {

		const float microsecondsPerFrame = (float)timeSinceEngineStart2->microseconds_ / framesCounter1->value_;
		const float msForFrame = microsecondsPerFrame / 1000.f;

		if (fPSPlotData0->microsecondsFromEngineStart_.size() < 2) {
			ImGui::Text("Current FPS: %.1f", 0);
		}
		else {
			auto previousFrame = fPSPlotData0->microsecondsFromEngineStart_.back();
			auto prePreviousFrame = fPSPlotData0->microsecondsFromEngineStart_[fPSPlotData0->microsecondsFromEngineStart_.size() - 2];
			auto fps = 1000.f / ((float)(previousFrame - prePreviousFrame) / 1000.f);
			ImGui::Text("Current FPS: %.1f",
				fps);
		}

		ImGui::Text("Average FPS: %.1f", 1000.f / msForFrame);
		ImGui::NewLine();
	}

	void ShowFPSPlot::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const ImPlotState* imPlotState0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0,
		const EnginePerformanceWindow* enginePerformanceWindow0,
		const EnginePerformance* enginePerformance0,
		const FPSPlotData* fPSPlotData0) {

		const Common::UInt64 framesNumber = fPSPlotData0->microsecondsFromEngineStart_.size();

		static std::vector<Common::UInt64> fpsPerFrame;

		fpsPerFrame.reserve(1000);

		for (Common::UInt64 i = 0; i < fPSPlotData0->microsecondsFromEngineStart_.size(); i++) {
			if (i == 0) {
				fpsPerFrame.push_back(0);
			}
			else {
				auto previousFrame = fPSPlotData0->microsecondsFromEngineStart_[i];
				auto prePreviousFrame = fPSPlotData0->microsecondsFromEngineStart_[i - 1];
				auto fps = 1000.f / ((float)(previousFrame - prePreviousFrame) / 1000.f);
				fpsPerFrame.push_back(fps);
			}
		}


		if (ImPlot::BeginPlot("My Plot")) {
			ImPlot::PlotLine("Frame time",
				fpsPerFrame.data(),
				fpsPerFrame.size(), 1.f, 0.f);
			//ImPlot::PlotBars("Frame time",
			//	fPSPlotData0->microsecondsFromEngineStart_.data(),
			//	fPSPlotData0->microsecondsFromEngineStart_.size(), 1.f, 0.f);
			
			ImPlot::EndPlot();
		}

		fpsPerFrame.clear();


	}

	void EndEnginePerformanceWindow::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImPlotState* imPlotState0,
		const EnginePerformanceMenuItem* enginePerformanceMenuItem0,
		const EnginePerformanceWindow* enginePerformanceWindow0, const EnginePerformance* enginePerformance0) {

		ImGui::End();

	}


} // namespace OksEngine