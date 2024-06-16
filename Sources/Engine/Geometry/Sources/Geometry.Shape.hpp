#pragma once

#include <Common.hpp>
#include <Geometry.IndexBuffer.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.Polygon.hpp>

namespace Geometry {

	template<class VertexType = Vertex3f, class IndexType = Index16>
	class Shape {
	public:

		Shape() noexcept = default;

		Shape( const VertexCloud<VertexType>& vertices,
				const IndexBuffer<IndexType>& indices) noexcept : 
			vertices_{ vertices }, 
			indices_{ indices } { }

		void AddPolygon(const Polygon<VertexType>& polygon) {
			const Common::Size startVerticesNumber = vertices_.GetVerticesNumber();
			for (Index16 i = 0; i < 3; i++) {
				vertices_.Add(polygon[i]);
				indices_.Add(startVerticesNumber + i);
			}
		}

		void Add(const Shape& shape) noexcept {

			const Common::Size beginVerticesNumber = vertices_.GetVerticesNumber();
			vertices_.Add(shape.vertices_.GetData(), shape.vertices_.GetVerticesNumber());

			const auto beginIndicesNumber = indices_.GetIndicesNumber();
			indices_.Add(shape.indices_);
			for (Common::Index i = beginIndicesNumber; i < indices_.GetIndicesNumber(); i++) {
				indices_[i] += static_cast<IndexType>(beginVerticesNumber);
			}

		}

		[[nodiscard]]
		const VertexCloud<VertexType>& GetVertices() const noexcept { return vertices_; }

		[[nodiscard]]
		const IndexBuffer<IndexType>& GetIndices() const noexcept { return indices_; }

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return vertices_.GetVerticesNumber(); }

		[[nodiscard]]
		Common::Size GetIndicesNumber() const noexcept { return indices_.GetIndicesNumber(); }

	private:
		VertexCloud<VertexType> vertices_;
		IndexBuffer<IndexType> indices_;
	};

	class Shape3f : public Shape<Vertex3f, Index16> {
	public:
	private:
		Color3f color_;
	};

	class Shape3fc : public Shape<Vertex3fc, Index16> {
	public:
	private:

	};

	class Shape3fnc : public Shape<Vertex3fnc, Index16> {

	};

	class Shape3fnct : public Shape<Vertex3fnct, Index16> {

	};

}