#pragma once 
#include <utility>
#include <Common.hpp>
#include <OS.Assert.hpp>

namespace Common::Memory {

	using Bytes = Common::Size;

	constexpr Bytes operator""_B(Common::Size value) noexcept {
		return value;
	}

	constexpr Bytes operator""_KB(Common::Size value) noexcept {
		return value * 1024;
	}

	constexpr Bytes operator""_MB(Common::Size value) noexcept {
		return value * 1_KB * 1_KB;
	}

	constexpr Bytes operator""_GB(Common::Size value) noexcept {
		return value * 1_MB * 1_MB;
	}

	[[nodiscard]]
	constexpr bool IsMemorySizeAligned(Common::Size memorySize) noexcept {
		return (memorySize % 8 == 0);
	}

	[[nodiscard]]
	constexpr Common::Size GetAlignedMemorySize(Common::Size memorySize) noexcept {

		if (IsMemorySizeAligned(memorySize)) {
			return memorySize;
		} else {
			const Common::Size remainder = memorySize % 8;
			OS::AssertMessage(remainder != 0, "Remainder can't be zero.");
			const Common::Size memorySizeWithAlignment = (memorySize - remainder + 8);
			OS::AssertMessage(IsMemorySizeAligned(memorySizeWithAlignment), "Aligned memory is not aligned.");
			return memorySizeWithAlignment;
		}

	}

	template<class Type, class ...Args>
	Type* Construct(void* p, Args&&... args) {
		return new (p) Type(std::forward<Args>(args)...);
	}

	template<class Type>
	void Destroy(void* p) {
		p->~Type();
	}
}