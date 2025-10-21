#pragma once


#include <Common.hpp>
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

		operator std::string() noexcept {
			return std::to_string(value_);
		}

		auto operator<=>(const Identifier& id) const noexcept = default;

		struct Hash {
			UInt64 operator()(const Identifier& id) const noexcept {
				return *this;
			}
		};

		Type GetValue() const noexcept {
			return value_;
		}

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
		Id Generate() noexcept {
			return ++lastId_;
		}

	private:
		Id lastId_ = 0;
	};

	using IdGenerator = IdentifierGenerator;

	template<class Type>
	class IdData {
	public:

		Type& operator[](Id id) {
			const auto idValue = id.GetValue();
			if (idValue >= idToData_.size()) {
				idToData_.resize(idValue * 2, Type{});
			}
			return idToData_[idValue];
		}

	private:

		//Index of the vector is id.
		std::vector<Type> idToData_;
	};

}