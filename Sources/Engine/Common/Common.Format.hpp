#pragma once 

#include <string>
#include <cstdarg>
#include <iostream>

#include <Common.Types.hpp>

namespace Common {

	class Format {
	public:

		Format(const char* format, ...) {

			static char buffer[1024 * 8];

			va_list(args);
			va_start(args, format);

			const int symbolsWrote = vsprintf_s(buffer, std::size(buffer), format, args);
			const bool isSuccess = (symbolsWrote >= 0);
			if (!isSuccess) {
				#pragma message ("Not processed error.")
			}
			va_end(args);

			text_ = buffer;

		}

		Format(const std::string& format, ...) : 
			Format{ format.c_str() } {

		}

		operator std::string() const {
			return text_;
		}

		operator std::string_view() const {
			return text_;
		}

	private:
		static inline constexpr Size maxBufferSize_ = 1024 * 8;
		std::string text_;
	};

	std::ostream& operator<<(std::ostream& os, const Format& format) {
		return os << static_cast<std::string>(format);
	}
}