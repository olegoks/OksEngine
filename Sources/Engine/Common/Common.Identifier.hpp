#pragma once

#include <limits>
#include <cstdint>
#include <functional>

#include <Common.Types.hpp>

#undef max
namespace Common {

	class [[nodiscard]] Identifier final {
	public:

		Identifier(const Identifier& copyId) noexcept : value_{ copyId.value_ } {}
		Identifier(Common::UInt64 value) noexcept : value_{ value } {}

		static Identifier Invalid() noexcept { return Max(); }
		static Identifier Max() noexcept { return Identifier{ std::numeric_limits<Common::UInt64>::max() }; }

		Identifier operator++(int value) noexcept { return Identifier{ value_++ }; }

		auto operator<=>(const Identifier& id) const noexcept = default;

		struct Hash {
			Common::UInt64 operator()(const Identifier& id) const noexcept {
				/*std::hash<Common::UInt64> hasher;
				return hasher(id.value_);*/
				return 0;
			}
		};

	private:
		Common::UInt64 value_ = Invalid().value_;

	};
}