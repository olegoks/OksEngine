#pragma once

#include <concepts>
#include <map>
#include <filesystem>
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
			if(!LuaCall(luaL_dostring(state_, script.text_.c_str()))){
				OS::AssertFailMessage("Error while loading config script.");
			}
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

		template<class Type>
		requires std::integral<Type>
		[[nodiscard]]
		bool IsTopStackValue() const noexcept {
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

		template<class Type>
		requires std::integral<Type>
		[[nodiscard]]
		Common::UInt64 ConvertStackTopValueTo() const noexcept {
			return lua_tonumber(state_, -1);
		}

		template<class Type>
		[[nodiscard]]

		Type GetGlobalAs(const char* variableName) const noexcept {
			char saveVariableScript[256] = "";
			/* Assign the Lua expression to a Lua global variable. */
			sprintf(saveVariableScript, "variable=%s", variableName);
			if(!LuaCall(luaL_dostring(state_, saveVariableScript))) {
				OS::AssertFailMessage({ "Attempt to get global value with name %s that is not correct.", variableName });
			}
			[[maybe_unused]]
			const int valueType = lua_getglobal(state_, "variable");
			OS::AssertMessage(IsTopStackValue<Type>(),
				"Attempt to get lua value with incorrect type conversation.");

			const Type configVariable = ConvertStackTopValueTo<Type>();
			lua_pop(state_, 1);
			return configVariable;
		}


		[[nodiscard]]
		bool LuaCall(int returnValue) const {
			if (returnValue) {
				OS::LogError("lua/", lua_tostring(state_, -1));
				lua_pop(state_, 1);
				return false;
			}
			return true;
		}
		~Context() noexcept {
			if (state_ != nullptr) {
				lua_close(state_);
			}
		}

	private:
		lua_State* state_ = nullptr;
	};

}