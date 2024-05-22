#pragma once

#include <exception>
#include <Common.Exception.hpp>
#include <Common.Format.hpp>

namespace OS {
	class Exception : public Common::Exception {
	public:
		Exception(Common::Format format);
		Exception(const char* message);

	};
}