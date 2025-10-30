#include <Common.hpp>

namespace Common
{
	NOINLINE void BreakPointLine() {}

	void BreakPoint() {
		
		__debugbreak();

	}
}