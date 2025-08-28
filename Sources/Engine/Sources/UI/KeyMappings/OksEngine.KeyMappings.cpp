#pragma once
#include <UI\KeyMappings\auto_OksEngine.KeyMappings.hpp>

namespace OksEngine
{
namespace UI
{
void EditKeyMappings(std::shared_ptr<ECS2::World> ecsWorld, KeyMappings *keyMappings) {};

void AddKeyMappings(ECS2::World *ecsWorld, ECS2::Entity::Id entityId) {};

void BindKeyMappings(::Lua::Context &context) {};

KeyMappings ParseKeyMappings(luabridge::LuaRef &keyMappingsRef) {
	
	return KeyMappings{};

};

std::string SerializeKeyMappings(const KeyMappings *keyMappings) {
	
	return "";
};

} // namespace UI

} // namespace OksEngine