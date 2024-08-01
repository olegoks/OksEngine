#pragma once 

#include <source_location>

#include <Common.hpp>

namespace OS {

	void AssertMessage(bool expression, Common::Format&& format, const std::source_location& location = std::source_location::current());

	void AssertFailMessage(Common::Format&& format, const std::source_location& location = std::source_location::current());

	void Assert(bool expression);
	void AssertFail();

	void NotImplemented(const std::source_location& location = std::source_location::current()) noexcept;

}