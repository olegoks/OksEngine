#pragma once

#include <filesystem>

#include <Common.hpp>
#include <OS.Assert.hpp>

namespace OksEngine
{
	class CommandLineParameters {
	public:
		CommandLineParameters(int argc, char** argv) noexcept :
			argc_{ argc },
			argv_{ argv } { }

		[[nodiscard]]
		std::filesystem::path GetExecutablePath() const {
			return argv_[0];
		}

		[[nodiscard]]
		std::string_view GetValue(const std::string_view valueName) const noexcept {
			for (int i = 0; i < argc_; ++i) {
				const std::string_view value = argv_[i];
				if (value == valueName) {
					const int nextValueIndex = i + 1;
					OS::Assert(nextValueIndex < argc_);
					return argv_[nextValueIndex];
				}
			}
			OS::AssertFail();
			return {};
		}

		[[nodiscard]]
		int GetArgc() const noexcept {
			return argc_;
		}

		[[nodiscard]]
		char** GetArgv() const noexcept {
			return argv_;
		}

	private:

	private:
		const int argc_ = 0;
		char** const argv_ = nullptr;
	};
}