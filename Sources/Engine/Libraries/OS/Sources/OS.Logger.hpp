#pragma once

#include <source_location>

#include <string>

#include <Common.hpp>
#include <Common.Format.hpp>
#include <iostream>
#include <filesystem>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace OS {

	class Logger {
	public:

		struct CreateInfo {
			int argc_ = 0;
			char** argv_ = nullptr;
		};

		class Severity {
		public:
			enum Type {
				Error,
				Warning,
				Info
			};

			Severity(Severity::Type severity) : type_{ severity } { }

			operator std::string () const noexcept {
				return magic_enum::enum_name(type_).data();
			}

			operator std::string_view() const noexcept {
				return magic_enum::enum_name(type_);
			}

			operator Type() const noexcept {
				return GetType();
			}

			[[nodiscard]]
			Type GetType() const noexcept { return type_; }

		private:
			Type type_;
		};

		Logger(CreateInfo& createInfo) {
			Common::DiscardUnusedParameter(createInfo);
			//loguru::init(createInfo.argc_, createInfo.argv_);
			//loguru::add_file("auto_engine.log", loguru::Append, loguru::Verbosity_MAX);


			spdlog::filename_t logFileName{ "auto_engine.log" };
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(logFileName);

			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

			std::vector<spdlog::sink_ptr> sinks{ file_sink, console_sink };

			logger_ = std::make_shared<spdlog::logger>("Logger",
				sinks.begin(),
				sinks.end());
		}

		void Log(Severity severity, const std::string_view path, const Common::Format& format, const std::source_location& location = std::source_location::current()) {
			Common::DiscardUnusedParameter(severity);
			Common::DiscardUnusedParameter(path);
			Common::DiscardUnusedParameter(location);
			Common::DiscardUnusedParameter(format);
			using namespace std::string_literals;

			const std::filesystem::path fullPath{ location.file_name() };

			const std::string fileName = fullPath.filename().generic_string();

			if (severity == Severity::Info) {
				logger_->info("File: "s + fileName + " " + static_cast<std::string>(format));
			}
			else if (severity == Severity::Error) {
				logger_->error("File: "s + fileName + " " + static_cast<std::string>(format));
			}
			else if (severity == Severity::Warning) {
				logger_->warn("File: "s + fileName + " " + static_cast<std::string>(format));
			}
			else {
				logger_->info("File: "s + fileName + " " + static_cast<std::string>(format));
			}
			//TODO: use severity
			
			
		}

	private:
		std::shared_ptr<spdlog::logger> logger_ = nullptr;
	};

	void InitializeLogger(int argc, char** argv);

	void Log(
		Logger::Severity severity, 
		const std::string_view path,
		Common::Format&& format,
		const std::source_location& location = std::source_location::current());

	void LogError(
		const std::string_view path, 
		Common::Format&& format,
		std::source_location location = std::source_location::current());

	void LogWarning(
		const std::string_view path,
		Common::Format&& format,
		std::source_location location = std::source_location::current());

	void LogInfo(
		const std::string_view path,
		Common::Format&& format,
		std::source_location location = std::source_location::current());

}
