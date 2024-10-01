#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LuaContext : public ECSComponent<LuaContext> {
		Lua::Context  context_;

		LuaContext(Lua::Context&& context);
	};


}