#pragma once 

#include <source_location>

#include <Common.hpp>
#include <format>

namespace OS {

	void AssertMessage(bool expression, const char* message, const std::source_location& location);

	template<class ...Args>
	void AssertMessage(bool expression, const std::source_location& location, const char* format, Args&& ...args) {

		if (expression) {
			return;
		}

		const std::string message = std::format(format, std::forward<Args&&>(args)...);
		AssertMessage(expression, message, location);

	}
 
	void AssertFailMessage(const char* message, const std::source_location& location = std::source_location::current());

	void Assert(bool expression);
	void AssertFail();

	void NotImplemented(const std::source_location& location = std::source_location::current()) noexcept;

}


#ifndef NDEBUG

#define ASSERT_MSG(expression, message)\
	OS::AssertMessage(expression, message, std::source_location::current())

#define ASSERT_FMSG(expression, message, ...)\
	OS::AssertMessage(expression, std::source_location::current(), message, __VA_ARGS__)

#define ASSERT(expression)\
	OS::Assert(expression)

#define ASSERT_FAIL()\
	OS::AssertFail()

#define NOT_IMPLEMENTED()\
	OS::NotImplemented()

#else

#define ASSERT_MSG(expression, message) ((void)0)

#define ASSERT_FMSG(expression, message, ...) ((void)0)

#define ASSERT(expression) ((void)0)

#define ASSERT_FAIL() ((void)0)

#define NOT_IMPLEMENTED() ((void)0)

#endif