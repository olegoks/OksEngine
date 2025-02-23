#pragma once

#include <Common.Types.hpp>

namespace Common {

	template<class Type>
	class [[nodiscard]] Identifier final {
	public:

		Identifier() = default;

		Identifier(const Identifier& copyId) noexcept : value_{ copyId.value_ } {
			
		}
		Identifier(Type value) noexcept : value_{ value } {}

		[[nodiscard]]
		static constexpr Identifier Invalid() noexcept { return Max(); }

		[[nodiscard]]
		static Identifier Max() noexcept { return Identifier{ Limits<Type>::Max() }; }

		Identifier operator++(int value) noexcept { return Identifier{ value_++ }; }

		Identifier operator++() noexcept { return Identifier{ ++value_ }; }


		auto operator<=>(const Identifier& id) const noexcept = default;

		struct Hash {
			UInt64 operator()(const Identifier& id) const noexcept {
				return *this;
			}
		};

		//[[nodiscard]]
		//Type operator Type() const noexcept {
		//	return value_;
		//}

		[[nodiscard]]
		bool IsInvalid() const noexcept { return (*this == Invalid()); }

	private:
		Type value_ = Invalid().value_;
	};

	using Id = Identifier<Common::UInt64>;

	class [[nodiscard]] IdentifierGenerator {
	public:
		IdentifierGenerator(Id startId) noexcept : lastId_{ startId } {

		}

		IdentifierGenerator() noexcept : lastId_{ 0 } {

		}

		[[nodiscard]]
		Id GenerateSystemDeclaration() noexcept {
			return ++lastId_;
		}

	private:
		Id lastId_ = 0;
	};

	using IdGenerator = IdentifierGenerator;

}