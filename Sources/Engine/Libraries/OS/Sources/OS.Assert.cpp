
#include <cassert>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>


namespace OS {

	void AssertMessage(bool expression, const char* message, const std::source_location& location) {

		if (expression) {
			return;
		}
		LogError("/assert/", message, location);
		//__debugbreak();
		assert(expression);

	}


	void AssertMessage(bool expression, const std::string& message, const std::source_location& location) {

		AssertMessage(expression, message.c_str(), location);
		
	}

	void AssertFailMessage(const char* message, const std::source_location& location) {
		AssertMessage(false, message, location);
	}

	void Assert(bool expression) {
		if (expression) {
			return;
		}
		assert(expression);
		__debugbreak();
	}

	void AssertFail() {
		Assert(false);
	}

	void NotImplemented(const std::source_location& location) noexcept {

		const std::string message = std::format(
			"Attempt to call not implemented function %s, file %s, line %d.",
			location.function_name(),
			location.file_name(),
			location.line()
		);

		AssertFailMessage(message.c_str());
	}

}