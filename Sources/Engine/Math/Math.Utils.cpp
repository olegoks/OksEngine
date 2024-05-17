#include <Math.Utils.hpp>

namespace Math {
    	Vector3f TransformPoint(const Vector3f& point, const Matrix4x4f& transform) {
		Matrix4x1f pointMatrix;
		{
			pointMatrix[0][0] = point[0];
			pointMatrix[1][0] = point[1];
			pointMatrix[2][0] = point[2];
			pointMatrix[3][0] = 1;
		}

		Matrix4x1f resultMatrix = pointMatrix * transform;

		Vector3f resultPoint;
		{
			resultPoint[0] = resultMatrix[0][0];
			resultPoint[1] = resultMatrix[1][0];
			resultPoint[2] = resultMatrix[2][0];
		}

		return resultPoint;
	}

	Vector3f TransformVector(const Vector3f& vector, const Matrix4x4f& transform) {
		Matrix4x1f pointMatrix;
		{
			pointMatrix[0][0] = vector[0];
			pointMatrix[1][0] = vector[1];
			pointMatrix[2][0] = vector[2];
			pointMatrix[3][0] = 0;
		}

		Matrix4x1f resultMatrix = pointMatrix * transform;

		Vector3f resultVector;
		{
			resultVector[0] = resultMatrix[0][0];
			resultVector[1] = resultMatrix[1][0];
			resultVector[2] = resultMatrix[2][0];
		}

		return resultVector;
	}
}