#pragma once 

#include <cstdint>
#include <limits>
#include <string>

namespace Common {

	using Unsigned = std::uint64_t;
	using Size = Unsigned;
	using Index = Unsigned;
	using Byte = char;

	using Float = float;
	using Double = double;

	using UInt8  = std::uint8_t;
	using UInt16 = std::uint16_t;
	using UInt32 = std::uint32_t;
	using UInt64 = std::uint64_t;

	using Int8 = std::int8_t;
	using Int16 = std::int16_t;
	using Int32 = std::int32_t;
	using Int64 = std::int64_t;

	constexpr static Common::Size cacheLineSize_ = 64;
	constexpr static Common::Size pointerSize = sizeof(void*);

	template<class Type>
	class [[nodiscard]] Limits {
	public:

		[[nodiscard]]
		constexpr static Type Max() noexcept {
#undef max
			return std::numeric_limits<Type>::max();
		}

		[[nodiscard]]
		constexpr static Type Min() noexcept {
#undef min
			return std::numeric_limits<Type>::min();
		}

	private:
	};

	using TypeId = std::uint64_t;

	template<class Type>
	class TypeInfo final {
	public:
		[[nodiscard]]
		TypeId GetId() const noexcept {
			return static_cast<TypeId>(typeid(Type).hash_code());
		}
		[[nodiscard]]
		const char* GetName() const noexcept {
			return typeid(Type).name();
		}
	};

}