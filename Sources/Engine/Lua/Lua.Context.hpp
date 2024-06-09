#pragma once

#include <map>
#include <filesystem>
#include <DataStructures.Vector.hpp>
#include <Lua.Common.hpp>

namespace Lua {

	class Context {
	private:
	public:

		class Script {
		public:
			std::string text_;
		};

		Context(Context&& moveContext) : 
			state_{ nullptr } {

			std::swap(state_, moveContext.state_);

		}

		Context() {
			state_ = luaL_newstate();
			OS::AssertMessage(state_ != nullptr, "Error while creating Lua context.");
			luaL_openlibs(state_);
			if (!LuaCall(lua_pcall(state_, 0, 0, 0), state_)) {
				//...
			}

		}

		Context& operator=(Context&& moveContext) {
			if (this == &moveContext) {
				return *this;
			}

			OS::AssertMessage(state_ != nullptr, 
				"Lua state must be not nullptr");
			lua_close(state_);
			state_ = nullptr;
			std::swap(state_, moveContext.state_);

			return *this;
		}

		void LoadScript(const Script& script) {
			OS::AssertMessage(state_ != nullptr, "");
			const bool isLoaded = LuaCall(luaL_dostring(state_, script.text_.c_str()), state_);

			OS::AssertMessage(isLoaded,	"Error while loading config script.");
		}

		[[nodiscard]]
		luabridge::Namespace GetGlobalNamespace() {
			return luabridge::getGlobalNamespace(state_);
		}

		template<class Type>
		[[nodiscard]]
		bool IsTopStackValue() const noexcept {
			OS::NotImplemented();
			return false;
		}

		template<>
		[[nodiscard]]
		bool IsTopStackValue<std::string>() const noexcept {
			OS::AssertMessage(state_ != nullptr, "State must be not nullptr.");
			return lua_isstring(state_, -1);
		}

		template<>
		[[nodiscard]]
		bool IsTopStackValue<Common::UInt64>() const noexcept {
			OS::AssertMessage(state_ != nullptr, "State must be not nullptr.");
			return lua_isnumber(state_, -1);
		}

		template<>
		[[nodiscard]]
		bool IsTopStackValue<bool>() const noexcept {
			OS::AssertMessage(state_ != nullptr, "State must be not nullptr.");
			return lua_istable(state_, -1);
		}

		template<class Type>
		[[nodiscard]]
		Type ConvertStackTopValueTo() const noexcept {
			OS::NotImplemented();
		}

		template<>
		[[nodiscard]]
		std::string ConvertStackTopValueTo<std::string>() const noexcept {
			return lua_tostring(state_, -1);
		}

		template<>
		[[nodiscard]]
		Common::UInt64 ConvertStackTopValueTo<Common::UInt64>() const noexcept {
			return lua_tonumber(state_, -1);
		}

		template<class Type>
		[[nodiscard]]
		Type GetGlobalAs(const char* variableName) const noexcept {
			char saveVariableScript[256] = "";
			/* Assign the Lua expression to a Lua global variable. */
			sprintf(saveVariableScript, "variable=%s", variableName);
			LuaCall(luaL_dostring(state_, saveVariableScript), state_);
			[[maybe_unused]]
			const int valueType = lua_getglobal(state_, "variable");
			//const bool istable = lua_istable(state_, -1);
			//const bool isuser = lua_isuserdata(state_, -1);
			//const bool isstring = lua_isstring(state_, -1);
			//const bool isbool = lua_isboolean(state_, -1);
			//const bool iscfunction = lua_iscfunction(state_, -1);
			//const bool isfunction = lua_isfunction(state_, -1);
			//const bool isinteger = lua_isinteger(state_, -1);
			//const bool islightuserdata = lua_islightuserdata(state_, -1);
			//const bool isnil = lua_isnil(state_, -1);
			OS::AssertMessage(IsTopStackValue<Type>(),
				"Attempt to get lua value with incorrect type conversation.");

			const Type configVariable = ConvertStackTopValueTo<Type>();
			lua_pop(state_, 1);
			return configVariable;
		}


		~Context() noexcept {
			OS::AssertMessage(state_, "");
			lua_close(state_);
		}

	private:
		lua_State* state_ = nullptr;
	};

}