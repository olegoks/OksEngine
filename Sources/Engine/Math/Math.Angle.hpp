#pragma once

#include <Common.hpp>
#include <Math.Common.hpp>
#include <OS.Assert.hpp>

namespace Math {

	class [[nodiscard]] Angle final {
	public:

		//180* == pi rad

		enum class Type {
			Degree,
			Radian
		};

		Angle(float value = 0.f, Type type = Type::Degree) noexcept :
			type_{ type },
			value_{ value } {

		}

		Angle(const Angle& copyAngle) noexcept = default;
		
		[[nodiscard]]
		inline float GetValue() const noexcept {
			return value_;
		}

		[[nodiscard]]
		inline Type GetType() const noexcept {
			return type_;
		}

		inline Angle& ToDegrees() noexcept {

			if (GetType() == Type::Degree) { return *this; }
			
			value_ = GetValue() / (pi / 180);
			type_ = Type::Degree;

			return *this;
		}

		inline Angle operator*(Common::UInt64 value) noexcept {
			value_ *= value;
			return *this;
		}

		inline Angle operator*(float value) noexcept {
			value_ *= value;
			return *this;
		}

		inline Angle& ToRadians() noexcept {
			
			if (GetType() == Type::Radian) { return *this; }

			value_ = GetValue() * (pi / 180);
			type_ = Type::Radian;

			return *this;
		}

		inline operator float() const noexcept {
			return GetValue();
		}

	private:
		Type type_;
		float value_ = 0.f;
	};

	[[nodiscard]]
	inline Angle operator""_degrees(Common::UInt64 degrees) noexcept {
		return Angle{ static_cast<float>(degrees) };
	}

	[[nodiscard]]
	inline Angle operator""_degree(Common::UInt64 degrees) noexcept {
		return Angle{ static_cast<float>(degrees) };
	}

	[[nodiscard]]
	inline Angle operator""_degrees(long double degrees) noexcept {
		return Angle{ static_cast<float>(degrees) };
	}

	[[nodiscard]]
	inline Angle operator""_degree(long double degrees) noexcept {
		return Angle{ static_cast<float>(degrees) };
	}


};