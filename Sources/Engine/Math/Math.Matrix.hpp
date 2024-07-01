#pragma once 

#include <array>
#include <variant>
#include <cmath>
#include <Math.Angle.hpp>
#include <Math.Vector.hpp>
#include <OS.Assert.hpp>
#include <Common.Types.hpp>

namespace Math {

	template<Common::Size columns, Common::Size rows, class Type>
	class [[nodiscard]] Matrix final {
	public:

		using Row = Vector<columns, Type>;
		using Column = Vector<rows, Type>;

		Matrix operator*(const Matrix& matrix) const noexcept {
			Matrix result;
			for (Common::Index x = 0; x < columns; x++) {
				const Column& column = matrix.GetColumn(x);
				for (Common::Index y = 0; y < rows; y++) {
					result[x][y] = column.DotProduct(GetRow(y));
				}
			}
			return result;
		}

		Matrix operator*(const Matrix<columns, columns, Type>& matrix) const requires(rows == 1){
			Matrix result;
			const Row& row = GetRow(0);
			for (Common::Index x = 0; x < columns; x++) {
				const Matrix<columns, columns, Type>::Column column = matrix.GetColumn(x);
				result[x][0] = row.DotProduct(column);
			}
			return result;
		}




		Column& operator[](Common::Index x) noexcept {
			return GetColumn(x);
		}

		const Column& operator[](Common::Index x) const noexcept {
			return const_cast<Matrix*>(this)->operator[](x);
		}

		static constexpr Matrix GetIdentity() noexcept requires(columns == rows) {
			Matrix matrix;
			for (Common::Index x = 0; x < columns; x++) {
				for (Common::Index y = 0; y < rows; y++) {
					if (x == y) { 
						matrix[x][y] = 1;
					}
				}
			}
			return matrix;
		}

		static constexpr Matrix GetTranslate(const Math::Vector<3, Type>& offset) noexcept requires (rows == 4 && columns == 4) {
			Matrix translate = Matrix::GetIdentity();

			translate[0][3] = offset.GetX();
			translate[1][3] = offset.GetY();
			translate[2][3] = offset.GetZ();
			
			return translate;

		}

		[[nodiscard]]
		Vector<3, Type> GetOrigin() const noexcept requires (rows == 4 && columns == 4) {
			Vector<4, Type> row = GetRow(3);
			return { row[0], row[1], row[2] };
		}

		static constexpr Matrix GetRotate(
			const Angle& angle, 
			const Math::Vector<3, Type>& vector) noexcept requires (rows == 4 && columns == 4) {
			
			Vector<rows - 1, Type> v = vector;
			v.Normalize();

			Angle radians = angle;
			if (radians.GetType() == Angle::Type::Degree) { radians.ToRadians(); }

			const Type c = std::cos(radians.GetValue());
			const Type s = std::sin(radians.GetValue());

			const Type x = v.GetX();
			const Type y = v.GetY();
			const Type z = v.GetZ();

			Matrix rotate = Matrix::GetIdentity();

			rotate[0][0] = c + (1 - c) * x * x; 
			rotate[0][1] = (1 - c) * y * x + s * z;
			rotate[0][2] = (1 - c) * z * x - s * y;

			rotate[1][0] = (1 - c) * x * y - s * z;
			rotate[1][1] = c + (1 - c) * y * y;
			rotate[1][2] = (1 - c) * z * y + s * x;

			rotate[2][0] = (1 - c) * x * z + s * y;
			rotate[2][1] = (1 - c) * y * z - s * x;
			rotate[2][2] = c + (1 - c) * z * z;

			return rotate;
		}

		static constexpr Matrix GetView(
			const Math::Vector<3, Type>& eye,
			const Math::Vector<3, Type>& direction,
			const Math::Vector<3, Type>& up) noexcept requires (rows == 4 && columns == 4) {

			const Math::Vector<3, Type> viewDir = direction;
			const Math::Vector<3, Type> right = viewDir.CrossProduct(up).Normalize();
			const Math::Vector<3, Type> down = viewDir.CrossProduct(right).Normalize();

			Matrix result = Matrix::GetIdentity();
			result[0][0] = right.GetX();
			result[1][0] = right.GetY();
			result[2][0] = right.GetZ();

			result[0][1] = down.GetX();
			result[1][1] = down.GetY();
			result[2][1] = down.GetZ();

			result[0][2] = -viewDir.GetX();
			result[1][2] = -viewDir.GetY();
			result[2][2] = -viewDir.GetZ();

			result[3][0] = -right.DotProduct(eye);
			result[3][1] = -down.DotProduct(eye);
			result[3][2] = viewDir.DotProduct(eye);

			return result;
		}

		static constexpr Matrix GetPerspective(const Angle& angle, Type ratio, Type zNear, Type zFar) noexcept requires (rows == 4 && columns == 4) {
			
			Angle radians = angle;
			if (radians.GetType() == Angle::Type::Degree) { radians.ToRadians(); }

			const Type tanHalfFovy = std::tan(radians.GetValue() / 2);

			Matrix result;
			result[0][0] = static_cast<Type>(1) / (ratio * tanHalfFovy);
			result[1][1] = static_cast<Type>(1) / (tanHalfFovy);
			result[2][2] = -(zFar + zNear) / (zFar - zNear);
			result[2][3] = -static_cast<Type>(1);
			result[3][2] = -(static_cast<Type>(2) * zFar * zNear) / (zFar - zNear);
			return result;
		}

		Column& GetColumn(Common::Index x) noexcept {
			return columns_[x];
		}

		const Column& GetColumn(Common::Index x) const noexcept {
			return const_cast<Matrix*>(this)->GetColumn(x);
		}

		Row GetRow(Common::Index y) const noexcept {
			Row row;
			for (Common::Index x = 0; x < columns; x++) {
				row[x] = (*this)[x][y];
			}
			return row;
		}

	private:
		std::array<Column, columns> columns_{ };
	};

	using Matrix4x1f = Matrix<4, 1, float>;
	using Matrix4x4f = Matrix<4, 4, float>;

}