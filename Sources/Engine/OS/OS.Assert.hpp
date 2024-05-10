#pragma once 

#include <string_view>
#include <source_location>

#include <OS.Logger.hpp>

namespace OS {

	void AssertMessage(bool expression, const Common::Format& format, const std::source_location& location = std::source_location::current()) {

		if (expression) {
			return;
		}
		LogError("/assert/", format, location);
		__debugbreak();
		#pragma message ("__debugbreak() is not cross platform decision.")
		terminate();
	}

	void AssertFailMessage(const Common::Format& format, const std::source_location& location = std::source_location::current()) {
		AssertMessage(false, format, location);
	}

	void Assert(bool expression, const std::source_location& location = std::source_location::current()) {
		AssertMessage(expression, "", location);
	}

	void NotImplemented(const std::source_location& location = std::source_location::current()) noexcept {
		AssertFailMessage({ "Attempt to call not implemented function %s, file %s, line %d.", location.function_name(), location.file_name(), location.line() });
	}

}