#pragma once

#include <Common.hpp>
#include <Geometry.Common.hpp>
#include <Geometry.IndexBuffer.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.Polygon.hpp>
#include <Geometry.Texture.hpp>
#include <Geometry.OBB.hpp>
#include <Geometry.AABB.hpp>

namespace Geometry {

	class Shape {
	public:

		enum class Type : Common::Size {
			Textured,
			Colored,
			Undefined
		};

		Shape() noexcept = default;

		Shape(  const VertexCloud<Vertex3f>&		vertices,
				const DS::Vector<Normal3f>			normals,
				const DS::Vector<UV2f>				uvs,
				const IndexBuffer<Index16>&			indices,
				std::shared_ptr<Geom::Texture<Geom::Color4b>> texture) noexcept :
			vertices_{ vertices },
			normals_{ normals },
			uvs_{ uvs },
			indices_{ indices }, texture_{ texture } { }

		Shape(const VertexCloud<Vertex3f>& vertices,
			const DS::Vector<Normal3f>			normals,
			const DS::Vector<Color3f>			colors,
			const IndexBuffer<Index16>& indices) noexcept :
			vertices_{ vertices },
			normals_{ normals },
			colors_{ colors },
			indices_{ indices } { }

		Shape(const VertexCloud<Vertex3f>& vertices,
			const IndexBuffer<Index16>& indices,
			const std::string& textureName) noexcept :

			textureName_{ textureName },
			vertices_{ vertices },
			indices_{ indices } { }

		void AddPolygon(const Polygon<Vertex3f>& polygon) {
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
				indices_[i] += static_cast<Index16>(beginVerticesNumber);
			}

		}

		[[nodiscard]]
		const VertexCloud<Vertex3f>& GetVertices() const noexcept { return vertices_; }

		[[nodiscard]]
		const IndexBuffer<Index16>& GetIndices() const noexcept { return indices_; }

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

		class Utils {
		public:
			static OBB GetObb(const Shape& shape) {
				
				float minX = Common::Limits<float>::Max();
				float maxX = Common::Limits<float>::Min();
				float minY = Common::Limits<float>::Max();
				float maxY = Common::Limits<float>::Min();
				float minZ = Common::Limits<float>::Max();
				float maxZ = Common::Limits<float>::Min();

				for (auto& vertex : shape.GetVertices()) {
					if(vertex.position_.GetX() > maxX){
						maxX = vertex.position_.GetX();
					}
					else if (vertex.position_.GetX() < minX) {
						minX = vertex.position_.GetX();
					}
					if (vertex.position_.GetY() > maxY) {
						maxX = vertex.position_.GetX();
					}
					else if (vertex.position_.GetY() < minY) {
						minY = vertex.position_.GetY();
					}
					if (vertex.position_.GetZ() > maxZ) {
						maxX = vertex.position_.GetZ();
					}
					else if (vertex.position_.GetZ() < minZ) {
						minZ = vertex.position_.GetZ();
					}
				}
			}
		};
		std::string textureName_ = "";
	private:
		VertexCloud<Vertex3f>	vertices_;
		DS::Vector<Normal3f>	normals_;
		DS::Vector<Color3f>		colors_;
		DS::Vector<UV2f>		uvs_;
		IndexBuffer<Index16>	indices_;


		std::shared_ptr<Texture<Color4b>> texture_ = nullptr;
	};


	

}