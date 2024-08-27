#pragma once 

#include <map>
#include <memory>

#include <Common.hpp>
#include <Common.Identifier.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.IndexBuffer.hpp>
#include <Geometry.Texture.hpp>
#include <Geometry.Polygon.hpp>

namespace Geometry {


	//class Shape {
	//public:

	//	Shape() noexcept = default;

	//	Shape(const VertexCloud<Vertex3f>& vertices,
	//		const DS::Vector<Normal3f>			normals,
	//		const DS::Vector<UV2f>				uvs,
	//		const IndexBuffer<Index16>& indices,
	//		std::shared_ptr<Geom::Texture<Geom::Color4b>> texture) noexcept :
	//		vertices_{ vertices },
	//		normals_{ normals },
	//		uvs_{ uvs },
	//		indices_{ indices }, texture_{ texture } { }

	//	Shape(const VertexCloud<Vertex3f>& vertices,
	//		const DS::Vector<Normal3f>			normals,
	//		const DS::Vector<Color3f>			colors,
	//		const IndexBuffer<Index16>& indices) noexcept :
	//		vertices_{ vertices },
	//		normals_{ normals },
	//		colors_{ colors },
	//		indices_{ indices } { }

	//	void AddPolygon(const Polygon<Vertex3f>& polygon) {
	//		const Common::Size startVerticesNumber = vertices_.GetVerticesNumber();
	//		for (Index16 i = 0; i < 3; i++) {
	//			vertices_.Add(polygon[i]);
	//			indices_.Add(startVerticesNumber + i);
	//		}
	//	}

	//	void Add(const Shape& shape) noexcept {

	//		const Common::Size beginVerticesNumber = vertices_.GetVerticesNumber();
	//		vertices_.Add(shape.vertices_.GetData(), shape.vertices_.GetVerticesNumber());

	//		const auto beginIndicesNumber = indices_.GetIndicesNumber();
	//		indices_.Add(shape.indices_);
	//		for (Common::Index i = beginIndicesNumber; i < indices_.GetIndicesNumber(); i++) {
	//			indices_[i] += static_cast<Index16>(beginVerticesNumber);
	//		}

	//	}

	//	[[nodiscard]]
	//	const VertexCloud<Vertex3f>& GetVertices() const noexcept { return vertices_; }
	//	[[nodiscard]]
	//	const IndexBuffer<Index16>& GetIndices() const noexcept { return indices_; }

	//	[[nodiscard]]
	//	Common::Size GetVerticesNumber() const noexcept { return vertices_.GetVerticesNumber(); }

	//	[[nodiscard]]
	//	Common::Size GetIndicesNumber() const noexcept { return indices_.GetIndicesNumber(); }

	//	void SetTexture(std::shared_ptr<Texture<Color4b>> texture) noexcept {
	//		texture_ = texture;
	//	}

	//	class Utils {
	//	public:
	//		static OBB GetObb(const Shape& shape) {

	//			float minX = Common::Limits<float>::Max();
	//			float maxX = Common::Limits<float>::Min();
	//			float minY = Common::Limits<float>::Max();
	//			float maxY = Common::Limits<float>::Min();
	//			float minZ = Common::Limits<float>::Max();
	//			float maxZ = Common::Limits<float>::Min();

	//			for (auto& vertex : shape.GetVertices()) {
	//				if (vertex.position_.GetX() > maxX) {
	//					maxX = vertex.position_.GetX();
	//				}
	//				else if (vertex.position_.GetX() < minX) {
	//					minX = vertex.position_.GetX();
	//				}
	//				if (vertex.position_.GetY() > maxY) {
	//					maxX = vertex.position_.GetX();
	//				}
	//				else if (vertex.position_.GetY() < minY) {
	//					minY = vertex.position_.GetY();
	//				}
	//				if (vertex.position_.GetZ() > maxZ) {
	//					maxX = vertex.position_.GetZ();
	//				}
	//				else if (vertex.position_.GetZ() < minZ) {
	//					minZ = vertex.position_.GetZ();
	//				}
	//			}
	//		}
	//	};
	//private:
	//	VertexCloud<Vertex3f>	vertices_;
	//	DS::Vector<Normal3f>	normals_;
	//	DS::Vector<Color3f>		colors_;
	//	DS::Vector<UV2f>		uvs_;
	//	IndexBuffer<Index16>	indices_;
	//	std::shared_ptr<Texture<Color4b>> texture_ = nullptr;
	//};

	class Mesh {
	public:

		class Id final {
		public:

			using ValueType = Common::Size;
			static const Id invalid_;

			Id() noexcept : value_{ invalid_ } { }
			Id(ValueType value) noexcept : value_{ value } {}

			operator ValueType() const noexcept {
				return value_;
			}

			Id operator++(int value) noexcept {
				return ++value_;
			}

			struct Hash {
				std::size_t operator()(const Id& id) const noexcept {
					const Id::ValueType value = id.operator size_t();
					return std::hash<Id::ValueType>{}(value);
				}
			};

			~Id() noexcept = default;

		private:
			ValueType value_;
		};



		VertexCloud<Vertex3f>	vertices_;
		DS::Vector<Normal3f>	normals_;
		DS::Vector<Color3f>		colors_;
		DS::Vector<UV2f>		uvs_;
		IndexBuffer<Index16>	indices_;
		std::shared_ptr<Texture> texture_ = nullptr;
	};
	//inline const Entity::Id Entity::Id::invalid_ = Common::Limits<Entity::Id::ValueType>::Max();

	VertexCloud<Vertex3fnt> GetVertexCloud3fnt(std::shared_ptr<Mesh> mesh) {

	}

	inline const Mesh::Id Mesh::Id::invalid_ = Common::Limits<ValueType>::Max();

	class TextureStorage {
	public:

		[[nodiscard]]
		Texture::Id AddTexture(std::shared_ptr<Texture> mesh) {
			Common::Maybe<Texture::Id> freeId = GetFreeId();
			OS::AssertMessage(freeId.has_value(), "There is no free id.");
			textures_[freeId.value()] = mesh;
			return freeId.value();
		}

		[[nodiscard]]
		bool IsTextureExist(Texture::Id meshId) {
			return textures_.find(meshId) != textures_.end();
		}

		[[nodiscard]]
		std::shared_ptr<Texture> GetTexture(Texture::Id meshId) {
			OS::AssertMessage(IsTextureExist(meshId), "Invalid mesh id.");
			return textures_.find(meshId)->second;
		}

		[[nodiscard]]
		Common::Maybe<Texture::Id> GetFreeId() noexcept {
			for (
				Texture::Id meshId = 0;
				meshId < 1000;
				meshId++) {
				const bool isIdFree = !IsTextureExist(meshId);
				if (isIdFree) {
					return meshId;
				}

			}
			return {};
		}
		std::map<Texture::Id, std::shared_ptr<Texture>> textures_;
	};


	class TaggedTextureStorage : private TextureStorage {
	public:

		struct CreateInfo {

		};

		TaggedTextureStorage(const CreateInfo& createInfo) {

		}

		[[nodiscard]]
		Texture::Id AddTexture(const std::string& tag, std::shared_ptr<Texture> texture) {
			const Texture::Id textureId = TextureStorage::AddTexture(texture);
			tagId_[tag] = textureId;
			return textureId;
		}

		[[nodiscard]]
		std::shared_ptr<Texture> GetTexture(Texture::Id textureId) {
			OS::AssertMessage(TextureStorage::IsTextureExist(textureId), "Invalid Texture id.");
			return TextureStorage::GetTexture(textureId);
		}

		[[nodiscard]]
		std::shared_ptr<Texture> GetTexture(const std::string& tag) {
			OS::AssertMessage(tagId_.find(tag) != tagId_.end(), "Invalid mesh id.");
			const Texture::Id textureId = tagId_[tag];
			return TextureStorage::GetTexture(textureId);
		}

		[[nodiscard]]
		bool IsTextureExist(const std::string& tag) {
			return tagId_.find(tag) != tagId_.end();
		}

		std::map<std::string, Texture::Id> tagId_;
	};

	class MeshStorage {
	public:

		[[nodiscard]]
		Mesh::Id AddMesh(std::shared_ptr<Mesh> mesh) {
			Common::Maybe<Mesh::Id> freeId = GetFreeId();
			OS::AssertMessage(freeId.has_value(), "There is no free id.");
			mesh_[freeId.value()] = mesh;
			return freeId.value();
		}

		[[nodiscard]]
		bool IsMeshExist(Mesh::Id meshId) {
			return mesh_.find(meshId) != mesh_.end();
		}

		[[nodiscard]]
		std::shared_ptr<Mesh> GetMesh(Mesh::Id meshId) {
			OS::AssertMessage(IsMeshExist(meshId), "Invalid mesh id.");
			return mesh_.find(meshId)->second;
		}

		[[nodiscard]]
		Common::Maybe<Mesh::Id> GetFreeId() noexcept {
			for (
				Mesh::Id meshId = 0;
				meshId < 1000;
				meshId++) {
				const bool isIdFree = !IsMeshExist(meshId);
				if (isIdFree) {
					return meshId;
				}

			}
			return {};
		}
		std::map<Mesh::Id, std::shared_ptr<Mesh>> mesh_;
	};


	class TaggedMeshStorage : private MeshStorage {
	public:

		struct CreateInfo {

		};

		TaggedMeshStorage(const CreateInfo& createInfo) {

		}

		[[nodiscard]]
		Mesh::Id AddMesh(const std::string& tag, std::shared_ptr<Mesh> mesh) {
			const Mesh::Id meshId = MeshStorage::AddMesh(mesh);
			tagId_[tag] = meshId;
			return meshId;
		}

		[[nodiscard]]
		std::shared_ptr<Mesh> GetMesh(Mesh::Id meshId) {
			OS::AssertMessage(MeshStorage::IsMeshExist(meshId), "Invalid mesh id.");
			return MeshStorage::GetMesh(meshId);
		}

		[[nodiscard]]
		std::shared_ptr<Mesh> GetMesh(const std::string& tag) {
			OS::AssertMessage(tagId_.find(tag) != tagId_.end(), "Invalid mesh id.");
			const Mesh::Id meshId = tagId_[tag];
			return MeshStorage::GetMesh(meshId);
		}

		[[nodiscard]]
		bool IsMeshExist(const std::string& tag) {
			return tagId_.find(tag) != tagId_.end();
		}

		std::map<std::string, Mesh::Id> tagId_;
	};

}