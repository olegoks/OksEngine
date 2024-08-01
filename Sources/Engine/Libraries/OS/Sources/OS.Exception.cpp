
#include <OS.Exception.hpp>

namespace OS {

    Exception::Exception(Common::Format format) : Common::Exception{format} {}
	Exception::Exception(const char* message) : Common::Exception{message} {}
}