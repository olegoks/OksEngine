#pragma once
#include <UI\KeyboardInput\auto_OksEngine.KeyboardInputSystems.hpp>

namespace OksEngine
{
	namespace Input {
		void CreateKeyboardEvents::Update(ECS2::Entity::Id entity0id, const Input::HandleKeyboardEvents* handleKeyboardEvents0) {

			CreateComponent<Input::KeyboardEvents>(entity0id);


		};

		void ClearKeyboardEvents::Update(ECS2::Entity::Id entity0id, Input::KeyboardEvents* keyboardEvents0) {

			keyboardEvents0->events_.clear();

		};

		void SendWindowKeyboardEvents::Update(
			ECS2::Entity::Id entity0id,
			const MainWindow* mainWindow0,
			const Input::KeyboardEvents* keyboardEvents0,
			ECS2::Entity::Id entity1id,
			const Input::HandleKeyboardEvents* handleKeyboardEvents1,
			Input::KeyboardEvents* keyboardEvents1) {

			for (auto& event : keyboardEvents0->events_) {
				keyboardEvents1->events_.push_back(event);
			}



		};

		void GetWindowKeyboardEvents::Update(ECS2::Entity::Id entity0id, MainWindow* mainWindow0,
			Input::KeyboardEvents* keyboardEvents0) {

			while (auto event = mainWindow0->window_->GetKeyboardEvent()) {
				static bool isCursorEnabled = false;
				//Debug code
				if (event.has_value()) {
					if (event.value().key_ == UIAL::Window::KeyboardKey::F5 && event.value().action_ == UIAL::Window::KeyAction::Pressed) {

						if (isCursorEnabled) {
							mainWindow0->window_->DisableCursor();
							isCursorEnabled = false;
						}
						else {
							mainWindow0->window_->EnableCursor();
							isCursorEnabled = true;
						}

					}
				}
				//if (isCursorEnabled) {
				//	continue;
				//}
				keyboardEvents0->events_.push_back({ event.value().key_, event.value().action_ });
			}



		};
	}
} // namespace OksEngine