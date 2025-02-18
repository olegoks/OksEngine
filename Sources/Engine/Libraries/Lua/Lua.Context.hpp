#pragma once

#include <concepts>
#include <map>
#include <filesystem>
#include <Lua.Common.hpp>
#include <Common.Identifier.hpp>
#include <OS.Assert.hpp>

namespace Lua {


	class Script {
	public:
		using Id = Common::Id;
		
		Script(const std::string& text);
		
		std::string text_;
	};


	class Context {
	private:
	public:

		Context() {
			state_ = luaL_newstate();
			OS::AssertMessage(state_ != nullptr, "Error while creating Lua context.");
			luaL_openlibs(state_);
		}

		Context(Context&& context) noexcept : state_{ nullptr } {
			std::swap(context.state_, state_);
		}

		Context& operator=(Context&& moveContext) noexcept {
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

		void ExecuteCode(const std::string& code) {
			if (!LuaCall(luaL_dostring(state_, code.c_str()))) {
				OS::AssertFailMessage({ "Error while executing code {}", code });
			}
		}

		template<class ...Args>
		void CallFunction(const std::string& functionName, Args&&... args) {
			luabridge::LuaRef function = luabridge::getGlobal(state_, functionName.c_str());
			function(std::forward(args)...);
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

		[[nodiscard]]
		bool IsTopStackValueUserData() const noexcept {
			OS::AssertMessage(state_ != nullptr, "State must be not nullptr.");
			return lua_isuserdata(state_, -1);
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
			return Type{};
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
		luabridge::LuaRef GetGlobalAsRef(const std::string& variableName) const noexcept {
			return luabridge::getGlobal(state_, variableName.c_str());
		}

		void AddPackagePath(const std::string& packagePath) {
			lua_getglobal(state_, "package");
			lua_getfield(state_, -1, "path"); // get field "path" from table at top of stack (-1)
			std::string cur_path = lua_tostring(state_, -1); // grab path string from top of stack
			cur_path.append(";"); // do your path magic here
			cur_path.append(packagePath + "?.lua");
			lua_pop(state_, 1); // get rid of the string on the stack we just pushed on line 5
			lua_pushstring(state_, cur_path.c_str()); // push the new one
			lua_setfield(state_, -2, "path"); // set the field "path" in table at -2 with value at top of stack
			lua_pop(state_, 1); // get rid of package table from top of stack
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