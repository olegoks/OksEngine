#pragma once
#include <Behaviour\auto_OksEngine.Behaviour.hpp>

namespace OksEngine
{
	namespace Behaviour
	{
		void CreateBehaviourLuaScript::Update(ECS2::Entity::Id entity0id, const Behaviour* behaviour0) {};

		void CallUpdateMethod::Update(ECS2::Entity::Id entity0id, const Behaviour* behaviour0, LuaContext* luaContext0) {};

		void CallInputProcessor::Update(ECS2::Entity::Id entity0id, const Behaviour* behaviour0, LuaContext* luaContext0,
			KeyboardInput* keyboardInput0, MouseInput* mouseInput0) {};

	} // namespace Behaviour

} // namespace OksEngine