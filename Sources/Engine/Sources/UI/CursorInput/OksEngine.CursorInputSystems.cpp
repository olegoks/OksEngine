#pragma once
#include <UI\CursorInput\auto_OksEngine.CursorInputSystems.hpp>

namespace OksEngine
{
	void CreateCursorEvents::Update(
		ECS2::Entity::Id entity0id,
		const HandleCursorEvents* handleCursorEvents0) {

		CreateComponent<CursorEvents>(entity0id);

	};

	void ClearCursorEvents::Update(
		ECS2::Entity::Id entity0id,
		CursorEvents* cursorEvents0) {
	

		cursorEvents0->events_ = std::queue<UIAL::Window::CursorEvent>{};
	};

	void SendWindowCursorEvents::Update(
		ECS2::Entity::Id entity0id, 
		const MainWindow* mainWindow0,
		const CursorEvents* cursorEvents0,
		
		ECS2::Entity::Id entity1id,
		const HandleCursorEvents* handleCursorEvents1,
		CursorEvents* cursorEvents1) {
	
		cursorEvents1->events_ = cursorEvents0->events_;

	};

	void GetWindowCursorEvents::Update(
		ECS2::Entity::Id entity0id, 
		MainWindow* mainWindow0, 
		CursorEvents* cursorEvents0) {
	
		while (auto event = mainWindow0->window_->GetCursorEvent()) {
			if (event.has_value()) {
				cursorEvents0->events_.push(event.value());
			}
		}
	
	};

} // namespace OksEngine