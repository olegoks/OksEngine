#pragma once

#include <utility>

#include <Common.Types.hpp>
#include <OS.Assert.hpp>

namespace Memory {

	using Bytes = Common::Size;

	constexpr inline Bytes operator""_B(Common::Size value) noexcept {
		return value;
	}

	constexpr inline Bytes operator""_KB(Common::Size value) noexcept {
		return value * 1024;
	}

	constexpr inline Bytes operator""_MB(Common::Size value) noexcept {
		return value * 1_KB * 1_KB;
	}

	constexpr inline Bytes operator""_GB(Common::Size value) noexcept {
		return value * 1_MB * 1_MB;
	}

	[[nodiscard]]
	constexpr inline bool IsMemorySizeAligned(Common::Size memorySize) noexcept {
		return (memorySize % 8 == 0);
	}

	[[nodiscard]]
	constexpr inline Common::Size GetAlignedMemorySize(Common::Size memorySize) noexcept;

	template<class Type, class ...Args>
	inline Type* Construct(void* p, Args&&... args) {
		return new (p) Type(std::forward<Args>(args)...);
	}

	template<class Type>
	inline void Destroy(void* p) {
		p->~Type();
	}

}