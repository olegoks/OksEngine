#pragma once

#include <array>
#include <cmath>
#include <type_traits>
#include <utility>

#include <Common.hpp>
#include <Math.Scalar.hpp>

namespace Math {

	template<Common::Size size, class Type>
	class [[nodiscard]] Vector {
	public:

		Vector() noexcept = default;

		Vector(const Type* data) noexcept {
			std::memcpy(data_.data(), data, GetSize());
		}

		//template<class ...Type>
		//Vector(Type&&... args) noexcept requires(std::is_floating_point_v<Type> || std::is_integral_v<Type>) : data_{std::forward<Type&&>(args)...} { }

		Vector(const Type x, const Type y) requires (size == 2)
			: data_{ x, y } { }

		Vector(const Type x, const Type y, const Type z) requires (size == 3)
			: data_{ x, y, z } { }

		Vector(const Type x, const Type y, const Type z, const Type w) requires (size == 4)
			: data_{ x, y, z, w } { }

		[[nodiscard]]
		Type& GetX() noexcept  requires (size >= 1) { return data_[0]; }

		[[nodiscard]]
		const Type& GetX() const noexcept requires (size >= 1) { return const_cast<Vector*>(this)->GetX(); }

		[[nodiscard]]
		Type& GetY() noexcept requires (size >= 2) { return data_[1]; }

		[[nodiscard]]
		const Type& GetY() const noexcept requires (size >= 2) { return const_cast<Vector*>(this)->GetY(); }

		[[nodiscard]]
		Type& GetZ() noexcept requires (size >= 3) { return data_[2]; }

		[[nodiscard]]
		const Type& GetZ() const noexcept requires (size >= 3) { return const_cast<Vector*>(this)->GetZ(); }

		[[nodiscard]]
		Type& GetW() noexcept requires (size >= 4) { return data_[3]; }

		[[nodiscard]]
		const Type& GetW() const noexcept requires (size >= 4) { return const_cast<Vector*>(this)->GetW(); }


		[[nodiscard]]
		Type DotProduct(const Vector& vector) const noexcept {
			Type result = 0;
			for (Common::Index elementIndex = 0;
				elementIndex < size;
				elementIndex++) {
				result += data_[elementIndex] * vector[elementIndex];
			}
			return result;
		}

		[[nodiscard]]
		Vector CrossProduct(const Vector& vector) const noexcept requires (size == 3) {
			Vector result;

			result[0] = GetY() * vector.GetZ() - GetZ() * vector.GetY();
			result[1] = GetZ() * vector.GetX() - GetX() * vector.GetZ();
			result[2] = GetX() * vector.GetY() - GetY() * vector.GetX();

			return result;
		}

		[[nodiscard]]
		Type& operator[](Common::Index elementIndex) noexcept {
			return data_[elementIndex];
		}

		[[nodiscard]]
		bool operator==(const Vector& vector) const noexcept requires (std::is_floating_point_v<Type>){
			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				if (!Math::IsEqual((*this)[elementIndex], vector[elementIndex])) {
					return false;
				}
			}
			return true;
		}

		[[nodiscard]]
		bool operator==(const Vector& vector) const noexcept requires (std::is_integral_v<Type>) {
			const int res = std::memcmp(data_.data(), vector.data_.data(), sizeof(data_));
			return res == 0;
		}

		[[nodiscard]]
		const Type& operator[](Common::Index elementIndex) const noexcept {
			return const_cast<Vector*>(this)->operator[](elementIndex);
		}

		Vector operator*(Type value) const noexcept {
			Vector result;

			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				result[elementIndex] = (*this)[elementIndex] * value;
			}
			return result;
		}

		Vector operator*(const Vector& vector) const noexcept {
			Vector result;

			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				result[elementIndex] = (*this)[elementIndex] * vector[elementIndex];
			}
			return result;
		}

		Vector operator-(const Vector& vector) const noexcept {
			Vector result;

			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				result[elementIndex] = (*this)[elementIndex] - vector[elementIndex];
			}
			return result;
		}

		Vector operator+(const Vector& vector) const noexcept {
			Vector result;

			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				result[elementIndex] = (*this)[elementIndex] + vector[elementIndex];
			}
			return result;
		}

		[[nodiscard]]
		glm::vec<size, Type> GetGlmVec() {
			return glm::vec<size, Type>{ data_.data() };
		}

		[[nodiscard]]
		static Vector Zero() { return Vector{}; }

		[[nodiscard]]
		static constexpr Common::Size GetElementSize() noexcept {
			return sizeof(Type);
		}

		[[nodiscard]]
		static constexpr Common::Size GetSize() noexcept {
			return sizeof(Type) * size;
		}

		[[nodiscard]]
		Type Length() const noexcept {
			Type sumOfSquares = 0;
			for (Common::Index elementIndex = 0; elementIndex < size; elementIndex++) {
				const Type element = (*this)[elementIndex];
				sumOfSquares += element * element;
			}
			return std::sqrt(sumOfSquares);
		}

		Vector& Normalize() noexcept {
			const Type length = Length();
			for (Type& element : data_) {
				element /= length;
			}
			return *this;
		}

		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vector& vector) const noexcept {
				std::hash<Type> hasher;
				Common::UInt64 hash = 0;
				for (Type& element : vector) {
					hash ^= hasher(element);
				}
				return hash;
			}
		};

		[[nodiscard]]
		Common::UInt64 GetHash() const noexcept { 
			Hash hasher;
			return hasher(*this);
		}

		using iterator = Type*;
		using const_iterator = const iterator;

		iterator begin() noexcept { return data_.data(); }
		iterator end() noexcept { return data_.data() + size; };
		
		const_iterator begin() const noexcept { return const_cast<Vector*>(this)->begin(); }
		const_iterator end() const noexcept { return const_cast<Vector*>(this)->end(); }

	private:
		std::array<Type, size> data_{ 0 };
	};

	using Vector2u64 = Vector<2, Common::UInt64>;
	using Vector2u32 = Vector<2, Common::UInt32>;
	using Vector2i = Vector<2, int>;
	using Vector2f = Vector<2, float>;
	using Vector2d = Vector<2, double>;
	using Vector3f = Vector<3, float>;
	using Vector3b = Vector<3, Common::Byte>;
	using Vector4b = Vector<4, Common::Byte>;
	using Vector4f = Vector<4, float>;

}