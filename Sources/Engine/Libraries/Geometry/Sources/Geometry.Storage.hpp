#pragma once 

#include <map>
#include <memory>

#include <Common.hpp>
#include <Common.Identifier.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.IndexBuffer.hpp>
#include <Geometry.Texture.hpp>
#include <Geometry.Polygon.hpp>
#include <Geometry.AABB.hpp>

namespace Geometry {

	class Mesh {
	public:

		using Id = Common::Id;

		[[nodiscard]]
		bool IsValid() const noexcept {
			OS::NotImplemented();
			return true;
		}
		Mesh() {}
		Mesh(const Mesh& copyMesh) = delete;
		Mesh& operator=(const Mesh& copyMesh) = delete;

		Mesh(Mesh&& moveMesh) = default;
		Mesh& operator=(Mesh&& moveMesh) = default;/* {
			if (this == &moveMesh) {
				return *this;
			}

			vertices_ = std::move(moveMesh.vertices_);
			normals_ = std::move(moveMesh.normals_);
			colors_ = std::move(moveMesh.colors_);
			uvs_ = std::move(moveMesh.uvs_);
			indices_ = std::move(moveMesh.indices_);
			stotextureId_ = std::move(moveMesh.textureId_);

			return *this;
		}*/

		VertexCloud<Vertex3f>	vertices_;
		DS::Vector<Normal3f>	normals_;
		DS::Vector<Color4f>		colors_;
		DS::Vector<UV2f>		uvs_;
		IndexBuffer<Index16>	indices_;
		//AABB aabb_;
		std::string textureName_;
		//std::string textureStorageTag_;
		//Texture::Id textureStorageId_;
		std::string name_;
		//RAL::Texture::Id driverTextureId_;
	};

	class Model2 {
	public:
		using Id = Common::Id;

		std::vector<Geom::Mesh> meshes_;
	};

	[[nodiscard]]
	inline VertexCloud<Vertex3fnt> GetVertexCloud3fnt(const Mesh& mesh) {
		VertexCloud<Vertex3fnt> vertices;
		for (Common::Index i = 0; i < mesh.vertices_.GetVerticesNumber(); i++) {
			Vertex3fnt vertex;
			vertex.position_ = mesh.vertices_[i].position_;
			vertex.normal_ = mesh.normals_[i];
			vertex.uv_ = mesh.uvs_[i];
			vertices.Add(vertex);
		}
		return vertices;
	}



}