#include <OS.Logger.hpp>

namespace OS {

	void LogError(const std::string_view path, const Common::Format format, std::source_location location) {
		logger.Log(Logger::Severity::Error, path, format, location);
	}

	void LogWarning(const std::string_view path, const Common::Format format, std::source_location location) {
		logger.Log(Logger::Severity::Warning, path, format, location);
	}

	void LogInfo(const std::string_view path, const Common::Format format, std::source_location location) {
		logger.Log(Logger::Severity::Info, path, format, location);
	}

}