#pragma once

#include <Common.Types.hpp>

namespace Common {

	template<class Type>
	class [[nodiscard]] Identifier final {
	public:

		Identifier(const Identifier& copyId) noexcept : value_{ copyId.value_ } {
			
		}
		Identifier(Type value) noexcept : value_{ value } {}

		[[nodiscard]]
		static Identifier Invalid() noexcept { return Max(); }

		[[nodiscard]]
		static Identifier Max() noexcept { return Identifier{ Limits<Type>::Max() }; }

		Identifier operator++(int value) noexcept { return Identifier{ value_++ }; }

		auto operator<=>(const Identifier& id) const noexcept = default;

		struct Hash {
			UInt64 operator()(const Identifier& id) const noexcept {
				return *this;
			}
		};

		[[nodiscard]]
		bool IsInvalid() const noexcept { return (*this == Invalid()); }

	private:
		Type value_ = Invalid().value_;

	};

	using Id = Identifier;
}