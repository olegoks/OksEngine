#include <OS.Memory.Common.hpp>

namespace Memory {

	[[nodiscard]]
	constexpr inline Common::Size GetAlignedMemorySize(Common::Size memorySize) noexcept {

		if (IsMemorySizeAligned(memorySize)) {
			return memorySize;
		}
		else {
			const Common::Size remainder = memorySize % 8;
			ASSERT_MSG(remainder != 0, "Remainder can't be zero.");
			const Common::Size memorySizeWithAlignment = (memorySize - remainder + 8);
			ASSERT_MSG(IsMemorySizeAligned(memorySizeWithAlignment), "Aligned memory is not aligned.");
			return memorySizeWithAlignment;
		}

	}
}