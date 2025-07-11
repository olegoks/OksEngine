#pragma once
#include <Debug\auto_OksEngine.ImGui.hpp>

#include <implot/implot.h>

namespace OksEngine
{

	void CreateImGuiState::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const KeyboardInput* keyboardInput0) {

		if (IsEntityExist<ImGuiState>()) {
			return;
		}

		bool isKeyPressed = false;
		for (auto& event : keyboardInput0->events_) {
			if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
				isKeyPressed = true;
				break;
			}
		}

		if (!isKeyPressed) {
			return;
		}


		ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<ImGuiState>(entityId);
		CreateComponent<HandleKeyboardInput>(entityId);

	};

	void CreateImPlotContext::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const HandleKeyboardInput* handleKeyboardInput0) {

		std::shared_ptr<ImPlotContext> implotState{
			ImPlot::CreateContext(), [](ImPlotContext* context) {

				ImPlot::DestroyContext(context);

			} };

		CreateComponent<ImPlotState>(entity0id, implotState);

	};


	void StartImGuiFrame::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0) {

		ImGui::NewFrame();

	};

	void EndImGuiFrame::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0) {

		ImGui::Render();

	};


	void DestroyImGuiState::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const KeyboardInput* keyboardInput0) {

		if (!IsEntityExist<ImGuiState>()) {
			return;
		}

		bool isKeyPressed = false;
		for (auto& event : keyboardInput0->events_) {
			if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
				isKeyPressed = true;
				break;
			}
		}

		if (!isKeyPressed) {
			return;
		}

		DestroyEntity(entity0id);

	};



} // namespace OksEngine