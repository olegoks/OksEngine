#pragma once

#include <filesystem>

#include <Common.hpp>
#include <OS.Assert.hpp>

namespace OS
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
		std::vector<std::string_view> GetValue(const std::string_view valueName) const noexcept {

#pragma region Assert
			OS::AssertMessage(valueName.starts_with('-'), "");
#pragma endregion

			std::vector<std::string_view> values;
			for (int i = 0; i < argc_; ++i) {
				const std::string_view parameter = argv_[i];
				if (parameter == valueName) {
					int j = i + 1;
					Assert(j < argc_);
					std::string_view value = argv_[j];
					while (!value.starts_with('-') ) {
						values.push_back(value);
						j++;
						if (j >= argc_) {
							break;
						}
						value = argv_[j];
					};
					return values;
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