
#include <Geometry.Model.hpp>

#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <Geometry.Storage.hpp>
#include <assimp/StringUtils.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

namespace Geometry {




	//template<class VertexType>
	//VertexType GetVertex(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
	//	OS::NotImplemented();
	//	return VertexType{};
	//}


	//template<>
	//Vertex3f GetVertex<Vertex3f>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
	//	Vertex3f vertex{};

	//	if (index.vertex_index >= 0) {
	//		vertex.position_ = {
	//			atrributes.vertices[3 * index.vertex_index + 0],
	//			atrributes.vertices[3 * index.vertex_index + 1],
	//			atrributes.vertices[3 * index.vertex_index + 2],
	//		};
	//	}
	//	return vertex;
	//}


	//template<>
	//Vertex3fnt GetVertex<Vertex3fnt>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
	//	Vertex3fnt vertex{};

	//	if (index.vertex_index >= 0) {
	//		vertex.position_ = {
	//			atrributes.vertices[3 * index.vertex_index + 0],
	//			atrributes.vertices[3 * index.vertex_index + 1],
	//			atrributes.vertices[3 * index.vertex_index + 2],
	//		};

	//		vertex.uv_ = {
	//			atrributes.texcoords[2 * index.texcoord_index + 0],
	//			1.0f - atrributes.texcoords[2 * index.texcoord_index + 1]
	//		};
	//	}

	//	if (index.normal_index >= 0) {
	//		vertex.normal_ = {
	//			atrributes.normals[3 * index.normal_index + 0],
	//			atrributes.normals[3 * index.normal_index + 1],
	//			atrributes.normals[3 * index.normal_index + 2],
	//		};
	//		vertex.normal_.Normalize();
	//	}
	//	return vertex;
	//}

	//template<>
	//inline Vertex3fnc GetVertex<Vertex3fnc>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
	//	Vertex3fnc vertex{};

	//	if (index.vertex_index >= 0) {
	//		vertex.position_ = {
	//			atrributes.vertices[3 * index.vertex_index + 0],
	//			atrributes.vertices[3 * index.vertex_index + 1],
	//			atrributes.vertices[3 * index.vertex_index + 2],
	//		};

	//		vertex.color_ = {
	//			atrributes.colors[3 * index.vertex_index + 0],
	//			atrributes.colors[3 * index.vertex_index + 1],
	//			atrributes.colors[3 * index.vertex_index + 2],
	//		};
	//	}

	//	if (index.normal_index >= 0) {
	//		vertex.normal_ = {
	//			atrributes.normals[3 * index.normal_index + 0],
	//			atrributes.normals[3 * index.normal_index + 1],
	//			atrributes.normals[3 * index.normal_index + 2],
	//		};
	//		vertex.normal_.Normalize();
	//	}
	//	return vertex;
	//}


	//Model<Vertex3fnc, Index16> ParseObj(const std::string& obj, const std::string& mtl) noexcept {

	//	tinyobj::ObjReader objReader;
	//	objReader.ParseFromString(obj, mtl);
	//	OS::AssertMessage(objReader.Valid(),
	//		"Attempt to use incorrect .obj/mtl model file.");
	//	const auto& attributes = objReader.GetAttrib();
	//	auto shapes = objReader.GetShapes();
	//	auto materials = objReader.GetMaterials();


	//	Model<Vertex3fnc, Index16> model;


	//	std::unordered_map<Vertex3fnc, Index16, Vertex3fnc::Hash> uniqueVertices{};
	//	for (const auto& objShape : shapes) {
	//		VertexCloud<Vertex3f> vertices;
	//		DS::Vector<Color3f> colors;
	//		DS::Vector<Normal3f> normals;
	//		IndexBuffer<Index16> indices;
	//		Common::Index vertexIndex = 0;
	//		for (const auto& index : objShape.mesh.indices) {
	//			auto vertex = GetVertex<Vertex3fnc>(index, attributes);
	//			auto& material = materials[objShape.mesh.material_ids[vertexIndex / 3]];
	//			vertex.color_ = Geom::Color3f{ material.diffuse[0], material.diffuse[1], material.diffuse[2] };
	//			bool deleteDuplicateVertices = true;
	//			if (deleteDuplicateVertices) {
	//				if (uniqueVertices.count(vertex) == 0) {
	//					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
	//					vertices.Add(Vertex3f{ vertex });
	//					normals.PushBack(vertex.normal_);
	//					colors.PushBack(vertex.color_);
	//				}
	//				indices.Add(uniqueVertices[vertex]);
	//			}
	//			else {
	//				vertices.Add(vertex);
	//				indices.Add(vertices.GetVerticesNumber() - 1);
	//			}
	//			vertexIndex++;
	//		}
	//		Shape shape{ vertices, normals, colors, indices };
	//		model.AddShape(shape);
	//	}
	//	OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
	//	return model;
	//}

	//Model<Vertex3fnt, Index16> ParseObjVertex3fnt(const std::string& obj, const std::string& mtl, const std::string& texture) noexcept {

	//	tinyobj::ObjReader objReader;
	//	objReader.ParseFromString(obj, mtl);
	//	OS::AssertMessage(objReader.Valid(),
	//		"Attempt to use incorrect .obj/mtl model file.");
	//	const auto& attributes = objReader.GetAttrib();
	//	auto shapes = objReader.GetShapes();
	//	auto materials = objReader.GetMaterials();


	//	Model<Vertex3fnt, Index16> model;


	//	//std::unordered_map<Vertex3fnt, Index16, Vertex3fnt::Hash> uniqueVertices{};
	//	//for (const auto& objShape : shapes) {
	//	//    VertexCloud<Vertex3f> vertices;
	//	//    DS::Vector<Normal3f> normals;
	//	//    DS::Vector<UV2f> uvs_;
	//	//    IndexBuffer<Index16> indices;
	//	//    Common::Index vertexIndex = 0;
	//	//    for (const auto& index : objShape.mesh.indices) {
	//	//        auto vertex = GetVertex<Vertex3fnt>(index, attributes);
	//	//        bool deleteDuplicateVertices = true;
	//	//        if (deleteDuplicateVertices) {
	//	//            if (uniqueVertices.count(vertex) == 0) {
	//	//                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
	//	//                vertices.Add(Vertex3f{ vertex });
	//	//                normals.PushBack(vertex.normal_);
	//	//                uvs_.PushBack(vertex.uv_);
	//	//            }
	//	//            indices.Add(uniqueVertices[vertex]);
	//	//        }
	//	//        else {
	//	//            vertices.Add(Vertex3f{ vertex });
	//	//            indices.Add(vertices.GetVerticesNumber() - 1);
	//	//        }
	//	//        vertexIndex++;
	//	//    }
	//	//
	//	//    auto textureObject = Geom::CreateTexture(texture.data(), texture.size());
	//	//    Shape shape{ vertices, normals, uvs_, indices,  textureObject };
	//	//    model.AddShape(shape);
	//	//}
	//	//OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
	//	return model;
	//}

	//Model<Vertex3f, Index16> ParseObjVertex3fIndex16(const std::string& obj) {

	//	tinyobj::ObjReader objReader;
	//	objReader.ParseFromString(obj, "");

	//	OS::AssertMessage(objReader.Valid(),
	//		"Attempt to use incorrect .obj/mtl model file.");

	//	const auto& attributes = objReader.GetAttrib();
	//	auto shapes = objReader.GetShapes();
	//	//auto materials = objReader.GetMaterials();


	//	Model<Vertex3f, Index16> model;

	//	for (const auto& objShape : shapes) {
	//		std::unordered_map<Vertex3f, Index16, Vertex3f::Hash> uniqueVertices{};
	//		VertexCloud<Vertex3f> vertices;
	//		IndexBuffer<Index16> indices;
	//		Common::Index vertexIndex = 0;
	//		for (const auto& index : objShape.mesh.indices) {
	//			auto vertex = GetVertex<Vertex3f>(index, attributes);

	//			glm::vec3 axis(1.0f, 0.0f, 0.0f);

	//			float angle = glm::radians(-90.0f);

	//			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	//			{
	//				glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex.position_.GetX(), vertex.position_.GetY(), vertex.position_.GetZ(), 1.0f);
	//				vertex.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//			}

	//			//auto& material = materials[objShape.mesh.material_ids[vertexIndex / 3]];
	//			//vertex.color_ = Geom::Color3f{ material.diffuse[0], material.diffuse[1], material.diffuse[2] };
	//			bool deleteDuplicateVertices = true;
	//			if (deleteDuplicateVertices) {
	//				if (uniqueVertices.count(vertex) == 0) {
	//					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
	//					vertices.Add(vertex);
	//				}
	//				indices.Add(uniqueVertices[vertex]);
	//			}
	//			else {
	//				vertices.Add(vertex);
	//				indices.Add(vertices.GetVerticesNumber() - 1);
	//			}
	//			vertexIndex++;
	//		}

	//		//auto textureObject = std::make_shared<Geom::Texture<Geom::Color4b>>(std::move(Geom::CreateTexture(texture.data(), texture.size())));
	//		//Shape shape{ vertices, indices ,""};
	//	   // model.AddShape(shape);
	//	}
	//	OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");

	//	OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
	//	return model;
	//}


	//Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16Textures(const std::string& obj, const std::string& mtl) {

	//    tinyobj::ObjReader objReader;
	//    objReader.ParseFromString(obj, mtl);

	//    OS::AssertMessage(objReader.Valid(),
	//        "Attempt to use incorrect .obj/mtl model file.");

	//    const auto& attributes = objReader.GetAttrib();
	//    auto shapes = objReader.GetShapes();
	//    auto materials = objReader.GetMaterials();


	//    Model<Vertex3fnt, Index16> model;

	//    for (int id = 0; id < materials.size(); id++) {
	//        const tinyobj::material_t& material = materials[id];
	//        VertexCloud<Vertex3f> vertices;
	//        DS::Vector<Normal3f> normals;
	//        DS::Vector<UV2f> uvs_;
	//        IndexBuffer<Index16> indices;
	//        for (const auto& objShape : shapes) {
	//            for (const auto verticesPerFace : objShape.mesh.num_face_vertices) {
	//                OS::Assert(verticesPerFace == 3);
	//            }
	//            for (std::size_t i = 0; i < objShape.mesh.material_ids.size(); i++) {
	//                const int materialId = objShape.mesh.material_ids[i];
	//                if (materialId == id) {
	//                    const std::size_t faceIndex = i;
	//                    const tinyobj::index_t Vertex1Index = objShape.mesh.indices[faceIndex * 3 + 0];
	//                    const tinyobj::index_t Vertex2Index = objShape.mesh.indices[faceIndex * 3 + 1];
	//                    const tinyobj::index_t Vertex3Index = objShape.mesh.indices[faceIndex * 3 + 2];
	//                    auto vertex1 = GetVertex<Vertex3fnt>(Vertex1Index, attributes);
	//                    auto vertex2 = GetVertex<Vertex3fnt>(Vertex2Index, attributes);
	//                    auto vertex3 = GetVertex<Vertex3fnt>(Vertex3Index, attributes);

	//                    glm::vec3 axis(1.0f, 0.0f, 0.0f);

	//                    float angle = glm::radians(-90.0f);

	//                    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	//                    {
	//                        glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex1.position_.GetX(), vertex1.position_.GetY(), vertex1.position_.GetZ(), 1.0f);
	//                        vertex1.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//                    }
	//                    {
	//                        glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex2.position_.GetX(), vertex2.position_.GetY(), vertex2.position_.GetZ(), 1.0f);
	//                        vertex2.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//                    }
	//                    {
	//                        glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex3.position_.GetX(), vertex3.position_.GetY(), vertex3.position_.GetZ(), 1.0f);
	//                        vertex3.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//                    }
	//                    vertices.Add(Vertex3f{ vertex1 });
	//                    normals.PushBack(vertex1.normal_);
	//                    uvs_.PushBack(vertex1.uv_);
	//                    const Index16 firstIndex = vertices.GetVerticesNumber() - 1;
	//                    vertices.Add(Vertex3f{ vertex2 });
	//                    normals.PushBack(vertex2.normal_);
	//                    uvs_.PushBack(vertex2.uv_);
	//                    vertices.Add(Vertex3f{ vertex3 });
	//                    normals.PushBack(vertex3.normal_);
	//                    uvs_.PushBack(vertex3.uv_);

	//                    indices.Add(firstIndex);
	//                    indices.Add(firstIndex + 1);
	//                    indices.Add(firstIndex + 2);
	//                }
	//            }

	//        }
	//       // Shape shape{ vertices, normals, uvs_, indices,  material.diffuse_texname };
	//        //shape.textureName_ = material.diffuse_texname;
	//        //model.AddShape(shape);
	//    }

	//    OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
	//    return model;
	//}


	//Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16Textures(const std::string& obj, const std::string& mtl) {

	//	tinyobj::ObjReader objReader;
	//	objReader.ParseFromString(obj, mtl);

	//	OS::AssertMessage(objReader.Valid(),
	//		"Attempt to use incorrect .obj/mtl model file.");

	//	const auto& attributes = objReader.GetAttrib();
	//	auto shapes = objReader.GetShapes();
	//	auto materials = objReader.GetMaterials();


	//	Model<Vertex3fnt, Index16> model;

	//	for (int id = 0; id < materials.size(); id++) {
	//		const tinyobj::material_t& material = materials[id];
	//		VertexCloud<Vertex3f> vertices;
	//		DS::Vector<Normal3f> normals;
	//		DS::Vector<UV2f> uvs_;
	//		IndexBuffer<Index16> indices;
	//		for (const auto& objShape : shapes) {
	//			for (const auto verticesPerFace : objShape.mesh.num_face_vertices) {
	//				OS::Assert(verticesPerFace == 3);
	//			}
	//			for (std::size_t i = 0; i < objShape.mesh.material_ids.size(); i++) {
	//				const int materialId = objShape.mesh.material_ids[i];
	//				if (materialId == id) {
	//					const std::size_t faceIndex = i;
	//					const tinyobj::index_t Vertex1Index = objShape.mesh.indices[faceIndex * 3 + 0];
	//					const tinyobj::index_t Vertex2Index = objShape.mesh.indices[faceIndex * 3 + 1];
	//					const tinyobj::index_t Vertex3Index = objShape.mesh.indices[faceIndex * 3 + 2];
	//					auto vertex1 = GetVertex<Vertex3fnt>(Vertex1Index, attributes);
	//					auto vertex2 = GetVertex<Vertex3fnt>(Vertex2Index, attributes);
	//					auto vertex3 = GetVertex<Vertex3fnt>(Vertex3Index, attributes);

	//					glm::vec3 axis(1.0f, 0.0f, 0.0f);

	//					float angle = glm::radians(-90.0f);

	//					glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	//					{
	//						glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex1.position_.GetX(), vertex1.position_.GetY(), vertex1.position_.GetZ(), 1.0f);
	//						vertex1.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//					}
	//					{
	//						glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex2.position_.GetX(), vertex2.position_.GetY(), vertex2.position_.GetZ(), 1.0f);
	//						vertex2.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//					}
	//					{
	//						glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex3.position_.GetX(), vertex3.position_.GetY(), vertex3.position_.GetZ(), 1.0f);
	//						vertex3.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
	//					}
	//					vertices.Add(Vertex3f{ vertex1 });
	//					normals.PushBack(vertex1.normal_);
	//					uvs_.PushBack(vertex1.uv_);
	//					const Index16 firstIndex = vertices.GetVerticesNumber() - 1;
	//					vertices.Add(Vertex3f{ vertex2 });
	//					normals.PushBack(vertex2.normal_);
	//					uvs_.PushBack(vertex2.uv_);
	//					vertices.Add(Vertex3f{ vertex3 });
	//					normals.PushBack(vertex3.normal_);
	//					uvs_.PushBack(vertex3.uv_);

	//					indices.Add(firstIndex);
	//					indices.Add(firstIndex + 1);
	//					indices.Add(firstIndex + 2);
	//				}
	//			}

	//		}
	//		//Shape shape{ vertices, normals, uvs_, indices,  material.diffuse_texname };
	//		//shape.textureName_ = material.diffuse_texname;
	//		//model.AddShape(shape);
	//	}

	//	OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
	//	return model;
	//}

	//Model<Vertex3fnc, Index16> ParseObjVertex3fncIndex16(const std::string& obj, const std::string& mtl)
	//{
	//	return ParseObj(obj, mtl);
	//}

	//Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16(const std::string& obj, const std::string& mtl, const std::string& image)
	//{
	//	return ParseObjVertex3fnt(obj, mtl, image);
	//}



	bool ParseModelFbx(const char* memory, Common::Size size) {

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFileFromMemory(memory, size,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType, "fbx");

		OS::AssertMessage(
			scene != nullptr,
			importer.GetErrorString());

		for (unsigned i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];
		}

		return true;
	}


	class MyIOStream : public Assimp::IOStream {
	public:
		MyIOStream(char* data, size_t length) : mData(data), mLength(length), mPos(0) { }

		size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override {
			size_t readSize = pSize * pCount;
			if (mPos + readSize > mLength) readSize = mLength - mPos;
			memcpy(pvBuffer, mData + mPos, readSize);
			mPos += readSize;
			return readSize;
		}

		size_t Write(const void* /*pvBuffer*/, size_t /*pSize*/, size_t /*pCount*/) override {
			return 0; // Writing is not supported
		}

		aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override {
			switch (pOrigin) {
			case aiOrigin_SET: mPos = pOffset; break;
			case aiOrigin_CUR: mPos += pOffset; break;
			case aiOrigin_END: mPos = mLength - pOffset; break;
			default: return aiReturn_FAILURE;
			}
			return aiReturn_SUCCESS;
		}

		size_t Tell() const override {
			return mPos;
		}

		size_t FileSize() const override {
			return mLength;
		}

		void Flush() override { }

	private:
		char* mData;
		size_t mLength;
		size_t mPos;
	};

	class ObjMtlIOSystem : public Assimp::IOSystem {
	public:

		ObjMtlIOSystem(const std::string& objName, const std::string& obj, const std::string& mtlName, const std::string mtl) :
			objName_{ objName },
			obj_{ obj },
			mtlName_{ mtlName },
			mtl_{ mtl } {

		}

		bool Exists(const char* fileName) const override {
			return fileName == objName_ || fileName == mtlName_;
		}

		char getOsSeparator() const override {
			return '/';  // Use forward slash as the separator
		}

		Assimp::IOStream* Open(const char* fileName, const char* /*pMode*/ = "rb") override {
			if (fileName == objName_) {
				return new MyIOStream(obj_.data(), obj_.size());
			}
			else if (fileName == mtlName_) {
				return new MyIOStream(mtl_.data(), mtl_.size());
			}
			else {
				return nullptr;
			}
		}

		void Close(Assimp::IOStream* pFile) override {
			delete pFile;
		}

		std::string objName_;
		std::string obj_;
		std::string mtlName_;
		std::string mtl_;

	};

	class FbxIOSystem : public Assimp::IOSystem {
	public:

		FbxIOSystem(const std::string& fbxName, const std::string& fbx) :
			fbxName_{ fbxName },
			fbx_{ fbx } {

		}

		bool Exists(const char* fileName) const override {
			return fileName == fbxName_;
		}

		char getOsSeparator() const override {
			return '/';  // Use forward slash as the separator
		}

		Assimp::IOStream* Open(const char* fileName, const char* /*pMode*/ = "rb") override {
			if (fileName == fbxName_) {
				return new MyIOStream(fbx_.data(), fbx_.size());
			}
			else {
				return nullptr;
			}
		}

		void Close(Assimp::IOStream* pFile) override {
			delete pFile;
		}

		std::string fbxName_;
		std::string fbx_;

	};


	[[nodiscard]]
	Geom::Model2 ParseFbxModelBaked(const std::string& fbxName, const std::string& fbx) {


		Assimp::Importer importer;

		importer.SetIOHandler(new FbxIOSystem{ fbxName, fbx });

		const aiScene* scene = importer.ReadFile(fbxName,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_MakeLeftHanded |
			aiProcess_FlipUVs |
			aiProcess_SortByPType);

		OS::AssertMessage(
			scene != nullptr,
			importer.GetErrorString());

		//Geom::Mesh geomMesh;
		Geom::Model2 model2;


		if (scene->HasAnimations()) {
			//OS::AssertMessage(scene->mNumAnimations <= 1, "More than one animation is not supported.");
			for (unsigned int i = 0; i < /*scene->mNumAnimations*/1; i++) {
				aiAnimation* animation = scene->mAnimations[i];
				auto animationPtr = std::make_shared<Model2::Animation>();
				animationPtr->ticksNumber_ = animation->mDuration;
				animationPtr->ticksPerSecond_ = animation->mTicksPerSecond;
				for (Common::Index j = 0; j < animation->mNumChannels; j++) {
					for (Common::Index ti = 0; ti < animation->mChannels[j]->mNumPositionKeys; ti++) {
						const aiVector3D& aiPosition = animation->mChannels[i]->mPositionKeys[ti].mValue;
						const aiQuaternion& aiRotation = animation->mChannels[i]->mRotationKeys[ti].mValue;
						Model2::Animation::StateInfo state{
							.time_ = animation->mChannels[i]->mPositionKeys[ti].mTime,
							.position_ = glm::vec3{ aiPosition.x ,aiPosition.y, aiPosition.z },
							.rotation_ = glm::quat::wxyz(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z)
						};
						
						animationPtr->states_.push_back(state);
					}
				}
				model2.animation_ = animationPtr;
			}
		}




		Common::UInt64 previousMeshIndicesNumber = 0;
		std::vector<Geom::Mesh> backedMeshs;
		for (unsigned i = 0; i < scene->mNumMeshes; i++) {

			const aiMesh* aimesh = scene->mMeshes[i];
			const auto materialPtr = scene->mMaterials[aimesh->mMaterialIndex];

			const int texturesNumber = materialPtr->GetTextureCount(aiTextureType_DIFFUSE);
			OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");

			//Process mesh only with texture.
			if (texturesNumber != 1) continue;

			aiString aiTexturePath;
			const aiReturn result = materialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
			OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
			const std::string textureName = aiTexturePath.C_Str();

			//Found mesh with the same texture name.
			Common::Index foundBackedMesh = Common::Limits<Common::Index>::Max();
			for (Common::Index i = 0; i < backedMeshs.size(); i++) {
				const Geom::Mesh& mesh = backedMeshs[i];
				if (mesh.textureName_ == textureName) {
					foundBackedMesh = i;
					break;
				}
			}

			//If there is no mesh with this name create new
			Geom::Mesh& backedMesh = (foundBackedMesh != Common::Limits<Common::Index>::Max()) ? (backedMeshs[foundBackedMesh]) : (backedMeshs.emplace_back());
			backedMesh.textureName_ = textureName;
			const Common::Size startVerticesNumber = backedMesh.vertices_.GetVerticesNumber();
			for (unsigned j = 0; j < aimesh->mNumVertices; j++) {
				backedMesh.vertices_.Add(Vertex3f{ aimesh->mVertices[j].x,
													aimesh->mVertices[j].y,
													aimesh->mVertices[j].z });
				backedMesh.normals_.PushBack(Normal3f{ aimesh->mVertices[j].x,
														aimesh->mVertices[j].y,
														aimesh->mVertices[j].z });
				backedMesh.uvs_.PushBack(UV2f{ aimesh->mTextureCoords[0][j].x,
												aimesh->mTextureCoords[0][j].y });
			}
			Geom::IndexBuffer meshIndices;
			meshIndices.Reserve(aimesh->mNumFaces * 3);
			for (unsigned j = 0; j < aimesh->mNumFaces; j++) {
				meshIndices.Add(aimesh->mFaces[j].mIndices[0]);
				meshIndices.Add(aimesh->mFaces[j].mIndices[1]);
				meshIndices.Add(aimesh->mFaces[j].mIndices[2]);
			}

			backedMesh.indices_.AddNextMesh(startVerticesNumber, meshIndices);
		}

		for (Geom::Mesh& mesh : backedMeshs) {
			model2.meshes_.push_back(std::move(mesh));
		}

		return model2;
	}

	[[nodiscard]]
	Geom::Model2 ParseObjMtlModelBaked(const std::string& objName, const std::string& obj, const std::string& mtlName, const std::string& mtl) {


		Assimp::Importer importer;

		importer.SetIOHandler(new ObjMtlIOSystem{ objName, obj, mtlName, mtl });

		const aiScene* scene = importer.ReadFile(objName,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_MakeLeftHanded |
			aiProcess_FlipUVs |
			aiProcess_SortByPType);

		OS::AssertMessage(
			scene != nullptr,
			importer.GetErrorString());

		//Geom::Mesh geomMesh;
		Geom::Model2 model2;
		Common::UInt64 previousMeshIndicesNumber = 0;
		std::vector<Geom::Mesh> backedMeshs;
		for (unsigned i = 0; i < scene->mNumMeshes; i++) {

			const aiMesh* aimesh = scene->mMeshes[i];
			const auto materialPtr = scene->mMaterials[aimesh->mMaterialIndex];

			const int texturesNumber = materialPtr->GetTextureCount(aiTextureType_DIFFUSE);
			OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");

			//Process mesh only with texture.
			if (texturesNumber != 1) continue;

			aiString aiTexturePath;
			const aiReturn result = materialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
			OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
			const std::string textureName = aiTexturePath.C_Str();

			//Found mesh with the same texture name.
			Common::Index foundBackedMesh = Common::Limits<Common::Index>::Max();
			for (Common::Index i = 0; i < backedMeshs.size(); i++) {
				const Geom::Mesh& mesh = backedMeshs[i];
				if (mesh.textureName_ == textureName) {
					foundBackedMesh = i;
					break;
				}
			}

			//If there is no mesh with this name create new
			Geom::Mesh& backedMesh = (foundBackedMesh != Common::Limits<Common::Index>::Max()) ? (backedMeshs[foundBackedMesh]) : (backedMeshs.emplace_back());
			backedMesh.textureName_ = textureName;

			const Common::Size startVerticesNumber = backedMesh.vertices_.GetVerticesNumber();

			backedMesh.vertices_.Reserve(backedMesh.vertices_.GetVerticesNumber() + aimesh->mNumVertices);
			backedMesh.normals_.Reserve(backedMesh.normals_.GetSize() + aimesh->mNumVertices);
			backedMesh.uvs_.Reserve(backedMesh.uvs_.GetSize() + aimesh->mNumVertices);

			for (unsigned j = 0; j < aimesh->mNumVertices; j++) {
				backedMesh.vertices_.Add(Vertex3f{ aimesh->mVertices[j].x,
													aimesh->mVertices[j].y,
													aimesh->mVertices[j].z });
				backedMesh.normals_.PushBack(Normal3f{ aimesh->mVertices[j].x,
														aimesh->mVertices[j].y,
														aimesh->mVertices[j].z });
				backedMesh.uvs_.PushBack(UV2f{ aimesh->mTextureCoords[0][j].x,
												aimesh->mTextureCoords[0][j].y });
			}
			Geom::IndexBuffer meshIndices;
			meshIndices.Reserve(aimesh->mNumFaces * 3);
			for (unsigned j = 0; j < aimesh->mNumFaces; j++) {
				meshIndices.Add(aimesh->mFaces[j].mIndices[0]);
				meshIndices.Add(aimesh->mFaces[j].mIndices[1]);
				meshIndices.Add(aimesh->mFaces[j].mIndices[2]);
			}

			backedMesh.indices_.AddNextMesh(startVerticesNumber, meshIndices);
		}

		for (Geom::Mesh& mesh : backedMeshs) {
			model2.meshes_.push_back(std::move(mesh));
		}

		return model2;
	}


	[[nodiscard]]
	Geom::Model2 ParseObjMtlModel(const std::string& objName, const std::string& obj, const std::string& mtlName, const std::string& mtl) {

		Assimp::Importer importer;

		importer.SetIOHandler(new ObjMtlIOSystem{ objName, obj, mtlName, mtl });

		const aiScene* scene = importer.ReadFile(objName,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		OS::AssertMessage(
			scene != nullptr,
			importer.GetErrorString());

		//Geom::Mesh geomMesh;
		Geom::Model2 model2;
		Common::UInt64 previousMeshIndicesNumber = 0;
		for (unsigned i = 0; i < scene->mNumMeshes; i++) {

			const aiMesh* mesh = scene->mMeshes[i];
			const auto materialPtr = scene->mMaterials[mesh->mMaterialIndex];

			const int texturesNumber = materialPtr->GetTextureCount(aiTextureType_DIFFUSE);
			OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");
			Geom::Mesh geomMesh;
			if (texturesNumber == 1) {
				aiString aiTexturePath;
				const aiReturn result = materialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
				OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
				const std::string textureFileName = aiTexturePath.C_Str();
				geomMesh.textureName_ = textureFileName;
			}
			const Common::Size startVerticesNumber = geomMesh.vertices_.GetVerticesNumber();
			geomMesh.vertices_.Reserve(mesh->mNumVertices);
			geomMesh.normals_.Reserve(mesh->mNumVertices);
			geomMesh.uvs_.Reserve(mesh->mNumVertices);
			for (unsigned j = 0; j < mesh->mNumVertices; j++) {
				geomMesh.vertices_.Add(Vertex3f{ mesh->mVertices[j].x,
													mesh->mVertices[j].y,
													mesh->mVertices[j].z });
				geomMesh.normals_.PushBack(Normal3f{ mesh->mVertices[j].x,
														mesh->mVertices[j].y,
														mesh->mVertices[j].z });
				if (texturesNumber == 1) {
					geomMesh.uvs_.PushBack(UV2f{ mesh->mTextureCoords[0][j].x,
													mesh->mTextureCoords[0][j].y });
				}
			}
			Geom::IndexBuffer meshIndices;
			meshIndices.Reserve(mesh->mNumFaces * 3);
			for (unsigned j = 0; j < mesh->mNumFaces; j++) {
				meshIndices.Add(mesh->mFaces[j].mIndices[0]);
				meshIndices.Add(mesh->mFaces[j].mIndices[1]);
				meshIndices.Add(mesh->mFaces[j].mIndices[2]);
			}
			geomMesh.indices_.AddNextMesh(startVerticesNumber, meshIndices);
			model2.meshes_.push_back(std::move(geomMesh));
		}

		return model2;
	}


}