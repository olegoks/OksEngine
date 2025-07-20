#pragma once
#include <Behaviour\auto_OksEngine.Behaviour.hpp>

namespace OksEngine
{
void CallInputProcessor::Update(ECS2::Entity::Id entity0id, const Behaviour *behaviour0, LuaContext *luaContext0,
                                KeyboardInput *keyboardInput0, MouseInput *mouseInput0) {};

void CallUpdateMethod::Update(ECS2::Entity::Id entity0id, const Behaviour *behaviour0, LuaContext *luaContext0) {};

} // namespace OksEngine