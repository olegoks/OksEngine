#pragma once 

#include <string_view>
#include <source_location>

#include <OS.Logger.hpp>

namespace OS {

	void AssertMessage(bool expression, const Common::Format& format, const std::source_location& location = std::source_location::current());

	void AssertFailMessage(const Common::Format& format, const std::source_location& location = std::source_location::current());

	void Assert(bool expression, const std::source_location& location = std::source_location::current());

	void NotImplemented(const std::source_location& location = std::source_location::current()) noexcept;

}