#pragma once

#include <Lua.hpp>
#include <luabridge3/LuaBridge/LuaBridge.h>
#include <OS.Logger.hpp>

namespace Lua {

	[[nodiscard]]
	inline bool LuaCall(int returnValue, lua_State* state) {
		if (returnValue) {
			OS::LogError("lua/", lua_tostring(state, -1));
			lua_pop(state, 1);
			return false;
		}
		return true;
	}

}