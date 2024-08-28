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

		Mesh(Mesh&& moveMesh) {

		}
		Mesh& operator=(Mesh&& moveMesh) {
			if (this == &moveMesh) {
				return *this;
			}

			vertices_ = std::move(moveMesh.vertices_);
			normals_ = std::move(moveMesh.normals_);
			colors_ = std::move(moveMesh.colors_);
			uvs_ = std::move(moveMesh.uvs_);
			indices_ = std::move(moveMesh.indices_);
			textureId_ = std::move(moveMesh.textureId_);

			return *this;
		}

		VertexCloud<Vertex3f>	vertices_;
		DS::Vector<Normal3f>	normals_;
		DS::Vector<Color3f>		colors_;
		DS::Vector<UV2f>		uvs_;
		IndexBuffer<Index16>	indices_;
		Texture::Id textureId_;
	};

	template<class Type>
	class [[nodiscard]] Storage {
	public:


		[[nodiscard]]
		Type::Id Add(Type&& object) {
			const Type::Id newId = idGenerator_.Generate();
			objects_.emplace(newId, std::move(object));
			return newId;
		}

		[[nodiscard]]
		bool IsExist(Type::Id objectId) {
			return objects_.find(objectId) != objects_.end();
		}

		[[nodiscard]]
		Type Get(Type::Id objectId) {
			OS::AssertMessage(IsExist(objectId), "Object is not exist.");
			return objects_.find(objectId)->second;
		}

	private:
		Common::IdGenerator idGenerator_;
		std::map<Common::Id, Type> objects_;
	};

	template<class Type>
	class [[nodiscard]] TaggedStorage : public Storage<Type>{
	public:

		[[nodiscard]]
		Type::Id Add(const std::string& tag, Type&& object) {
			const Type::Id objectId = Storage<Type>::Add(object);
			tagId_[tag] = objectId;
			return objectId;
		}

		[[nodiscard]]
		Type& Get(Type::Id objectId) {
			OS::AssertMessage(Storage<Type>::IsExist(objectId), "Invalid object id.");
			return Storage<Type>::Get(objectId);
		}

		[[nodiscard]]
		Type& Get(const std::string& tag) {
			OS::AssertMessage(tagId_.find(tag) != tagId_.end(), "Invalid object id.");
			const Type::Id objectId = tagId_[tag];
			return Storage<Type>::Get(objectId);
		}

		[[nodiscard]]
		bool IsExist(const std::string& tag) {
			return tagId_.find(tag) != tagId_.end();
		}

		std::map<std::string, Common::Id> tagId_;
	};

	using TaggedMeshStorage = TaggedStorage<Mesh>;
	using TaggedTextureStorage = TaggedStorage<Texture>;


	VertexCloud<Vertex3fnt> GetVertexCloud3fnt(std::shared_ptr<Mesh> mesh) {
		return VertexCloud<Vertex3fnt>{};
	}



}