#pragma once
#include <UI\MouseInput\auto_OksEngine.MouseInputSystems.hpp>

namespace OksEngine
{
	void CreateMouseEvents::Update(ECS2::Entity::Id entity0id, const HandleMouseEvents* handleMouseInput0) {

		CreateComponent<MouseEvents>(entity0id);
	};

	void ClearMouseEvents::Update(ECS2::Entity::Id entity0id, MouseEvents* mouseInput0) {

		mouseInput0->events_ = std::queue<UIAL::Window::MouseEvent>{};
	};

	void GetWindowMouseEvents::Update(ECS2::Entity::Id entity0id, MainWindow* mainWindow0, MouseEvents* mouseInput0) {

		while (auto event = mainWindow0->window_->GetMouseEvent()) {
			if (event.has_value()) {
				mouseInput0->events_.push(event.value());
			}
		}

	};

	void SendWindowMouseEvents::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const MouseEvents* mouseInput0,

		ECS2::Entity::Id entity1id,
		const HandleMouseEvents* handleMouseInput1,
		MouseEvents* mouseInput1) {

		if (!mouseInput0->events_.empty()) {
			mouseInput1->events_ = mouseInput0->events_;
		}
		

	};

} // namespace OksEngine