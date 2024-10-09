#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {



	struct LuaScript : public ECSComponent<LuaScript> {
		LuaScript() : ECSComponent{ nullptr } {}

	};


	template<>
	void Edit<LuaScript>(LuaScript* script);

	template<>
	void Add<LuaScript>(ECS::World* world, ECS::Entity::Id id);


}