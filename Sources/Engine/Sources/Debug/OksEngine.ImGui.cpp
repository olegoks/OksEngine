#pragma once
#include <Debug\auto_OksEngine.ImGui.hpp>

#include <implot/implot.h>

namespace OksEngine
{

	void CreateImGuiState::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const Input::KeyboardEvents* keyboardInput0) {

		//if (IsEntityExist<ImGuiState>()) {
		//	return;
		//}

		//bool isKeyPressed = false;
		//for (auto& event : keyboardInput0->events_) {
		//	if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
		//		isKeyPressed = true;
		//		break;
		//	}
		//}

		//if (!isKeyPressed) {
		//	return;
		//}


		ECS2::Entity::Id entityId = CreateEntity();

#if !defined(NDEBUG)
		CreateComponent<ImGuiState>(entityId);
		CreateComponent<Input::HandleKeyboardEvents>(entityId);
#endif
	};

	void CreateImPlotContext::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const Input::HandleKeyboardEvents* handleKeyboardInput0) {

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
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);
		ImGui::Render();

	};


	void DestroyImGuiState::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const Input::KeyboardEvents* keyboardInput0) {

		if (!IsEntityExist<ImGuiState>()) {
			return;
		}

		bool isKeyPressed = false;
		for (auto& event : keyboardInput0->events_) {
			//if (event.first == UIAL::Window::KeyboardKey::F5 && event.second == UIAL::Window::KeyboardAction::Pressed) {
			//	isKeyPressed = true;
			//	break;
			//}
		}

		if (!isKeyPressed) {
			return;
		}

	};



} // namespace OksEngine