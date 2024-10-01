#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {



	struct LuaScript : public ECSComponent<LuaScript> {
		std::string tag_;
		Lua::Script::Id id_;

		LuaScript(const std::string& tag, Lua::Script::Id scriptId);

	};


	template<>
	void Edit<LuaScript>(LuaScript* script);

	template<>
	void Add<LuaScript>(ECS::World* world, ECS::Entity::Id id);


}