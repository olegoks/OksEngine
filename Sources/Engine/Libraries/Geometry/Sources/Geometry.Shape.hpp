#pragma once

#include <Common.hpp>
#include <Geometry.Common.hpp>
#include <Geometry.IndexBuffer.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.Polygon.hpp>
#include <Geometry.Texture.hpp>

namespace Geometry {

	template<class VertexType = Vertex3f, class IndexType = Index16>
	class Shape {
	public:

		enum class Type : Common::Size {
			Textured,
			Colored,
			Undefined
		};

		Shape() noexcept = default;

		Shape( const VertexCloud<VertexType>& vertices,
				const IndexBuffer<IndexType>& indices,
				std::shared_ptr<Texture<Color4b>> texture = nullptr) noexcept : 
			vertices_{ vertices }, 
			indices_{ indices },
			texture_{ texture } { }

		Shape(const VertexCloud<VertexType>& vertices,
			const IndexBuffer<IndexType>& indices,
			const std::string& textureName) noexcept :
			vertices_{ vertices },
			indices_{ indices },
			textureName_{ textureName } { }

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

		[[nodiscard]]
		Type GetType() const noexcept {
			if (colors_.GetSize() > 0) {
				return Type::Colored;
			} else if (texture_ != nullptr) {
				return Type::Textured;
			}
			OS::NotImplemented();
			return Type::Undefined;
		}

		void SetTexture(std::shared_ptr<Texture<Color4b>> texture) noexcept {
			texture_ = texture;
		}

		[[nodiscard]]
		std::shared_ptr<Texture<Color4b>> GetTexture() const noexcept {
			return texture_;
		}
		std::string textureName_ = "";
	private:
		VertexCloud<VertexType> vertices_;
		IndexBuffer<IndexType> indices_;
		DS::Vector<Normal3f> normals_;
		DS::Vector<Color3f> colors_;

		std::shared_ptr<Texture<Color4b>> texture_ = nullptr;
	};

	//class FlexShape {
	//public:

	//	void AddVertex(const Vertex3f) {
	//		
	//	}

	//private:
	//	DS::Vector<Vertex3f> vertices_;
	//	DS::Vector<Color3f> colors_;
	//	DS::Vector<Normal3f> normals_;
	//	DS::Vector<UV2f> uvs_;
	//};

}