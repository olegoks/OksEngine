#pragma once

#include <Engine.hpp>
#include <Lua.Context.hpp>

namespace OksEngine {

	struct Config {
		std::filesystem::path resourcesPath_;

		Config(Config&& moveConfig) : 
			context_{ std::move(moveConfig.context_) } {

		}

		Config(const std::string& text) {
			::Lua::Script script{ text };
			context_.LoadScript(script);
		}

		template<class Type>
		[[nodiscard]]
		Type GetValueAs(const char* valuePath) const noexcept {
			return context_.GetGlobalAs<Type>(valuePath);
		}

	private:
		::Lua::Context context_;
	};

}