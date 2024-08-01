#pragma once 

#include <format>
#include <iostream>
#include <string>
#include <cstdarg>
#include <iostream>

#include <Common.Types.hpp>

namespace Common {

	class Format {
	public:

		template<class ...Args>
		Format(const char* format, Args&& ...args) {
			text_ = std::vformat(format, std::make_format_args(args...));
		}

		Format(const std::string format) {
			text_ = format;
		}

		Format(const char* format) {
			text_ = format;
		}


		operator std::string() const {
			return text_;
		}

		operator std::string_view() const {
			return text_;
		}

	private:
		std::string text_;
	};

	std::ostream& operator<<(std::ostream& os, const Format& format);
}