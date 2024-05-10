#pragma once 

#include <cstdint>
#include <limits>

namespace Common {

	using Unsigned = std::uint64_t;
	using Size = Unsigned;
	using Index = Unsigned;
	using Byte = char;

	using Float = float;
	using Double = double;

	using UInt16 = std::uint16_t;
	using UInt32 = std::uint32_t;
	using UInt64 = std::uint64_t;

	constexpr static Common::Size pointerSize = sizeof(void*);

	template<class Type>
	class [[nodiscard]] Limits {
	public:

		[[nodiscard]]
		constexpr static Type Max() noexcept {
			return std::numeric_limits<Type>::max();
		}

	private:
	};

	using TypeInfoId = std::uint64_t;

	template<class Type>
	class TypeInfo final {
	public:
		[[nodiscard]]
		TypeInfoId GetId() const noexcept {
			return static_cast<TypeInfoId>(typeid(Type).hash_code());
		}
	};

}