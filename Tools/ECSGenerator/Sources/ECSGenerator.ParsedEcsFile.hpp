#pragma once

#include <ECSGenerator.Common.hpp>

namespace ECSGenerator{


	class ParsedECSFile {
	public:
		enum class Type {
			Component,
			System,
			Undefined
		};

		ParsedECSFile(const std::filesystem::path& path, const std::string& name) :
			path_{ path },
			name_{ name } {  }


		[[nodiscard]]
		virtual Type GetType() const noexcept = 0;

		[[nodiscard]]
		const std::string GetName() const noexcept {
			return name_;
		}

		[[nodiscard]]
		const std::filesystem::path GetPath() const noexcept {
			return path_;
		}

	private:

		const std::filesystem::path path_;
		const std::string name_;
	};

    
}