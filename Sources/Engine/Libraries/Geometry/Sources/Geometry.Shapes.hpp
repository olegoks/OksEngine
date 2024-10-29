#pragma once

#include <array>

#include <Geometry.Common.hpp>
#include <Geometry.Vertex.hpp>
#include <Geometry.ConvexHull.hpp>

namespace Geometry {

	//template<class IndexType>
	//class Box : public ConvexHull<Vertex3f, IndexType> {
	//public:
	//	Box(Length edge) noexcept {

	//		const Length halfLength = edge / 2.f;
	//		
	//		const std::array<Vertex3f, 8> vertices {
	//			Vertex3f{ -halfLength, -halfLength, -halfLength },
	//			Vertex3f{ halfLength, -halfLength, -halfLength },
	//			Vertex3f{ halfLength,  halfLength, -halfLength },
	//			Vertex3f{ -halfLength,  halfLength, -halfLength },
	//			Vertex3f{ -halfLength, -halfLength,  halfLength },
	//			Vertex3f{ halfLength, -halfLength,  halfLength },
	//			Vertex3f{ halfLength,  halfLength,  halfLength },
	//			Vertex3f{ -halfLength,  halfLength,  halfLength }
	//		};

	//		const std::array<IndexType, 36> indices {
	//			0, 1, 2,
	//			2, 3, 0,
	//			1, 5, 6,
	//			6, 2, 1,
	//			4, 7, 6,
	//			6, 5, 4,
	//			3, 7, 4,
	//			4, 0, 3,
	//			4, 5, 1,
	//			1, 0, 4,
	//			3, 2, 6,
	//			6, 7, 3 
	//		};

	//		VertexCloud<Vertex3f> vertexCloud{};
	//		vertexCloud.Add(vertices.data(), vertices.size());

	//		IndexBuffer<IndexType> indexBuffer{};
	//		indexBuffer.Add(indices.data(), indices.size());

	//		Shape shape{ vertexCloud, indexBuffer };

	//		ConvexHull<Vertex3f, IndexType>::Add(shape);

	//	}

	//private:
	//	Length edge_ = 0;
	//};
}