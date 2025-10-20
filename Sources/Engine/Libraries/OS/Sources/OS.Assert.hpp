#pragma once 

#include <source_location>

#include <format>
#include <Common.hpp>

namespace OS {

	void AssertMessage(bool expression, const char* message, const std::source_location& location);

	template<class ...Args>
	void AssertMessage(bool expression, const std::source_location& location, const char* formatString, Args&& ...args) {

		if (expression) {
			return;
		}

		const std::string message = ::std::vformat(formatString, std::make_format_args(std::forward<Args>(args)...));


		AssertMessage(expression, message.c_str(), location);

	}
 
	void AssertFailMessage(const char* message, const std::source_location& location = std::source_location::current());
	
	template<class ...Args>
	void AssertFailMessage(const std::source_location& location, const char* format, Args&& ...args) {
		
		const std::string message = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
		AssertFailMessage(message.c_str(), location);

	}

	void Assert(bool expression);
	void AssertFail();

	void NotImplemented(const std::source_location& location = std::source_location::current()) noexcept;

}

//#define USE_ASSERTS


#define STATIC_ASSERT_MSG(expression, message)\
	static_assert(expression, message)

#if !defined(NDEBUG) || defined(USE_ASSERTS) 


#define ASSERT_MSG(expression, message)\
	OS::AssertMessage(expression, message, std::source_location::current())

#define ASSERT_FMSG(expression, message, ...)\
	OS::AssertMessage(expression, std::source_location::current(), message, __VA_ARGS__)

#define ASSERT(expression)\
	OS::AssertMessage(expression, #expression, std::source_location::current())

#define ASSERT_FAIL()\
	OS::AssertFail()

#define ASSERT_FAIL_MSG(message)\
	OS::AssertFailMessage(message)

#define ASSERT_FAIL_FMSG(format, ...)\
	OS::AssertFailMessage(std::source_location::current(), format,  __VA_ARGS__)

#define NOT_IMPLEMENTED()\
	OS::NotImplemented()

#else

#define ASSERT_MSG(expression, message) ((void)0)

#define ASSERT_FMSG(expression, message, ...) ((void)0)

#define ASSERT(expression) ((void)0)

#define ASSERT_FAIL() ((void)0)

#define ASSERT_FAIL_MSG(message) ((void)0)

#define ASSERT_FAIL_FMSG(message, ...)  ((void)0)

#define NOT_IMPLEMENTED() ((void)0)

#endif