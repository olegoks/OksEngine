#include <cassert>
#include <format>

#include <OS.Assert.hpp>

#include <OS.Logger.hpp>

namespace OS {

	void AssertMessage(bool expression, Common::Format&& format, const std::source_location& location) {

		if constexpr (!Common::IsDebug()) {
			return;
		}
		if (expression) {
			return;
		}
		LogError("/assert/", std::move(format), location);
		__debugbreak();
		assert(expression);

	}

	void AssertFailMessage(Common::Format&& format, const std::source_location& location) {
		AssertMessage(false, std::move(format), location);
	}

	void Assert(bool expression) {
		if (!Common::IsDebug()) {
			return;
		}
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
		AssertFailMessage({ "Attempt to call not implemented function %s, file %s, line %d.", location.function_name(), location.file_name(), location.line() });
	}

}