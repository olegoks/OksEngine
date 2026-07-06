#pragma once 

#include <ECS2.Component.hpp>
#include <Lua.Common.hpp>

template <class Component>
void Edit(std::shared_ptr<ECS2::World>, ECS2::Entity::Id entityId, Component* component) { }

template <class Component>
std::string Serialize(Component* component) { return ""; };

template <class Component>
Component Parse(luabridge::LuaRef& luaRef) { return Component{}; };

template <class Component>
void Bind(lua_State* state) { };

namespace OksEngine {

}