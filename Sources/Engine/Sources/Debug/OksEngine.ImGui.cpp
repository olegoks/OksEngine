#pragma once
#include <Debug\auto_OksEngine.ImGui.hpp>

#include <implot/implot.h>

namespace OksEngine
{
	namespace ImGUI {
		void CreateState::Update(
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
			CreateComponent<State>(entityId);
			CreateComponent<Input::HandleKeyboardEvents>(entityId);
#endif
		};

		void CreateImPlotContext::Update(
			ECS2::Entity::Id entity0id,
			const State* imGuiState0,
			const Input::HandleKeyboardEvents* handleKeyboardInput0) {

			std::shared_ptr<ImPlotContext> implotState{
				ImPlot::CreateContext(), [](ImPlotContext* context) {

					ImPlot::DestroyContext(context);

				} };

			CreateComponent<ImPlotState>(entity0id, implotState);

		};


		void StartFrame::Update(
			ECS2::Entity::Id entity0id,
			const State* imGuiState0) {

			::ImGui::NewFrame();

		};

		void EndFrame::Update(
			ECS2::Entity::Id entity0id,
			const State* imGuiState0) {
			::ImGui::Render();

		};


		void DestroyState::Update(
			ECS2::Entity::Id entity0id,
			const State* imGuiState0,
			const Input::KeyboardEvents* keyboardInput0) {

			if (!IsEntityExist<State>()) {
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

	}

} // namespace OksEngine