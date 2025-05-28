#pragma once


#include <Lua.Context.hpp>

namespace OksEngine {

	struct ConfigFile {
		std::filesystem::path resourcesPath_;

		ConfigFile(ConfigFile&& moveConfig) : 
			context_{ std::move(moveConfig.context_) } {

		}

		ConfigFile(const std::string& text) {
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