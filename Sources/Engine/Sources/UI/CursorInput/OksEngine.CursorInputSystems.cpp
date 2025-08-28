#pragma once
#include <UI\CursorInput\auto_OksEngine.CursorInputSystems.hpp>

namespace OksEngine
{
void ClearCursorEvents::Update(ECS2::Entity::Id entity0id, CursorEvents *cursorEvents0) {};

void CreateCursorEvents::Update(ECS2::Entity::Id entity0id, const HandleCursorEvents *handleCursorEvents0) {};

void SendWindowCursorEvents::Update(ECS2::Entity::Id entity0id, const MainWindow *mainWindow0,
                                    const CursorEvents *cursorEvents0, ECS2::Entity::Id entity1id,
                                    const HandleCursorEvents *handleCursorEvents1, CursorEvents *cursorEvents1) {};

void GetWindowCursorEvents::Update(ECS2::Entity::Id entity0id, MainWindow *mainWindow0, CursorEvents *cursorEvents0) {};

} // namespace OksEngine