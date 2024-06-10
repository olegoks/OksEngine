#include <OS.Logger.hpp>

#include <mutex>
#include <memory>

namespace OS {


	std::once_flag initilizeLoggerFlag_;
	std::unique_ptr<Logger> logger_ = nullptr;

	void InitializeLogger(int argc, char** argv) {
		std::call_once(initilizeLoggerFlag_, [&]()
		{
			OS::Assert(argc != 0);
			Logger::CreateInfo createInfo;
			{
				createInfo.argc_ = argc;
				createInfo.argv_ = argv;
			}
			logger_ = std::make_unique<Logger>(createInfo);
		});
	}

	void Log(Logger::Severity severity, const std::string_view path, Common::Format&& format, const std::source_location& location) {
		InitializeLogger(0, nullptr);
		logger_->Log(severity, path, format, location);
	}

	void LogError(const std::string_view path, Common::Format&& format, std::source_location location) {
		Log(Logger::Severity::Error, path, std::move(format), location);
	}

	void LogWarning(const std::string_view path, Common::Format&& format, std::source_location location) {
		Log(Logger::Severity::Warning, path, std::move(format), location);
	}

	void LogInfo(const std::string_view path, Common::Format&& format, std::source_location location) {
		Log(Logger::Severity::Info, path, std::move(format), location);
	}

}