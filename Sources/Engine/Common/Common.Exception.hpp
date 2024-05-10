#pragma once 

#include <exception>
#include <string>

#include <Common.Format.hpp>

namespace Common {

	class Exception : std::exception {
	public:
		Exception(Format format) : message_{ format } { }
		Exception(const char* message) : message_{ message } { }

		const std::string& GetMessage() const { return message_; }

	private:
		const std::string message_;
	};
}