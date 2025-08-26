#pragma once
#include <UI\MouseInput\auto_OksEngine.MouseInputSystems.hpp>

namespace OksEngine
{
	void CreateMouseInput::Update(ECS2::Entity::Id entity0id, const HandleMouseInput* handleMouseInput0) {

		CreateComponent<MouseInput>(entity0id);
	};

	void ClearMouseInput::Update(ECS2::Entity::Id entity0id, MouseInput* mouseInput0) {

		mouseInput0->events_ = std::queue<UIAL::Window::MouseEvent>{};
	};

	void GetWindowMouseInput::Update(ECS2::Entity::Id entity0id, MainWindow* mainWindow0, MouseInput* mouseInput0) {

		while (auto event = mainWindow0->window_->GetMouseEvent()) {
			if (event.has_value()) {
				mouseInput0->events_.push(event.value());
			}
		}

	};

	void SendWindowMouseInput::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const MouseInput* mouseInput0,

		ECS2::Entity::Id entity1id,
		const HandleMouseInput* handleMouseInput1,
		MouseInput* mouseInput1) {

		mouseInput1->events_ = mouseInput0->events_;

	};

} // namespace OksEngine