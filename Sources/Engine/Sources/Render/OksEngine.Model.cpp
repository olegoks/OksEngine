#pragma once
#include <Render\auto_OksEngine.Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace OksEngine
{

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

	class GltfIOSystem : public Assimp::IOSystem {
	public:

		GltfIOSystem(std::shared_ptr<AsyncResourceSubsystem> resourceSystem) :
			resourceSystem_{ resourceSystem } {

		}

		bool Exists(const char* fileName) const override {

			//resourceSystem_->
			//return fileName == gltfName_;
			return true;
		}

		char getOsSeparator() const override {
			return '/';  // Use forward slash as the separator
		}

		Assimp::IOStream* Open(const char* fileName, const char* /*pMode*/ = "rb") override {
			auto data = resourceSystem_->GetResourceSynch(Subsystem::Type::ChildThread, "Root/" + std::string(fileName));
			data_ = std::move(data.data_);
			return new MyIOStream(data_.GetData(), data_.GetSize());
		}

		void Close(Assimp::IOStream* pFile) override {
			delete pFile;
		}

		std::shared_ptr<AsyncResourceSubsystem> resourceSystem_;
		DS::Vector<Common::Byte> data_;
		Common::Size size_;
	};

	void CreateModel::Update(
		ECS2::Entity::Id entity0id,
		const Model* model0,
		const ModelFile* modelFile0,

		ECS2::Entity::Id entity1id,
		const ResourceSystem* resourceSystem1) {


		Resources::ResourceData resourceData = resourceSystem1->system_->GetResourceSynch(
			Subsystem::Type::ChildThread,
			"Root/" + modelFile0->fileName_);

		Assimp::Importer importer;

		importer.SetIOHandler(new GltfIOSystem{ resourceSystem1->system_ });

		// Флаги обработки
		const unsigned int flags =
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes;

		// Чтение файла из памяти
		const aiScene* scene = importer.ReadFileFromMemory(
			resourceData.GetData<Common::Byte>(),
			resourceData.GetSize(),
			flags,
			"gltf"
		);

		auto errorString = std::string(importer.GetErrorString());

		if (!scene || !scene->mRootNode) {
			throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
		}

		auto createMeshComponents = [this](const aiScene* scene, aiNode* node, ECS2::Entity::Id entityId) {

			Geom::VertexCloud<Geom::Vertex3f>   vertices;
			DS::Vector<Geom::Normal3f>	        normals;
			DS::Vector<Geom::Color4b>		    colors;
			DS::Vector<Geom::UV2f>		        uvs;
			Geom::IndexBuffer<Geom::Index16>	indices;

			for (Common::Index i = 0; i < node->mNumMeshes; i++) {
				int meshIndex = node->mMeshes[i];
				aiMesh* mesh = scene->mMeshes[meshIndex];

				vertices.Reserve(mesh->mNumVertices);
				normals.Reserve(mesh->mNumVertices);
				uvs.Reserve(mesh->mNumVertices);
				for (unsigned j = 0; j < mesh->mNumVertices; j++) {
					vertices.Add(Geometry::Vertex3f{ mesh->mVertices[j].x,
														mesh->mVertices[j].y,
														mesh->mVertices[j].z });
					normals.PushBack(Geom::Normal3f{ mesh->mVertices[j].x,
															mesh->mVertices[j].y,
															mesh->mVertices[j].z });
					uvs.PushBack(Geom::UV2f{ mesh->mTextureCoords[0][j].x,
													mesh->mTextureCoords[0][j].y });
				}

				indices.Reserve(mesh->mNumFaces * 3);
				for (unsigned j = 0; j < mesh->mNumFaces; j++) {
					indices.Add(mesh->mFaces[j].mIndices[0]);
					indices.Add(mesh->mFaces[j].mIndices[1]);
					indices.Add(mesh->mFaces[j].mIndices[2]);

				}
			}

			CreateComponent<Vertices3D>(entityId, vertices);
			CreateComponent<Normals>(entityId, normals);
			CreateComponent<UVs>(entityId, uvs);
			CreateComponent<Indices>(entityId, indices);

			};

		std::function<void(const aiScene*, aiNode*)> processChildrenNode
			= [this, &processChildrenNode, &createMeshComponents](const aiScene* scene, aiNode* node) {

			const ECS2::Entity::Id nodeEntityId = CreateEntity();

			createMeshComponents(scene, node, nodeEntityId);

			for (Common::Index i = 0; i < node->mNumChildren; i++) {
				aiNode* childrenNode = node->mChildren[i];
				processChildrenNode(scene, childrenNode);
			}

			};


		createMeshComponents(scene, scene->mRootNode, entity0id);
		for (Common::Index i = 0; i < scene->mRootNode->mNumChildren; i++) {
			aiNode* childrenNode = scene->mRootNode->mChildren[i];
			processChildrenNode(scene, childrenNode);
		}

		// Common::UInt64 previousMeshIndicesNumber = 0;
		// for (unsigned i = 0; i < scene->mNumMeshes; i++) {

		//     const aiMesh* aimesh = scene->mMeshes[i];
		//     const auto materialPtr = scene->mMaterials[aimesh->mMaterialIndex];

		//     const int texturesNumber = materialPtr->GetTextureCount(aiTextureType_DIFFUSE);
		//     OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");


		//     //Process mesh only with texture.
		//     if (texturesNumber != 1) continue;

		//     aiString aiTexturePath;
		//     const aiReturn result = materialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		//     OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
		//     std::string textureName = aiTexturePath.C_Str();
		//     const Common::Index slashIndex = textureName.rfind('\\');
		//     if (slashIndex != -1) {
		//         textureName = textureName.substr(slashIndex + 1);
		//     }
		//     //If there is no mesh with this name create new
		//     //Geom::Model2::Mesh& backedMesh = /*(foundBackedMesh != Common::Limits<Common::Index>::Max()) ? (backedMeshs[foundBackedMesh]) : (*/backedMeshs.emplace_back()/*)*/;

		//     //Parse animation
		//     //backedMesh.name_ = aimesh->mName.C_Str();


		//     //backedMesh.mesh_.textureName_ = textureName;
		//     //backedMesh.mesh_.vertices_.Reserve(aimesh->mNumVertices);
		//     //backedMesh.mesh_.normals_.Reserve(aimesh->mNumVertices);
		//     //backedMesh.mesh_.uvs_.Reserve(aimesh->mNumVertices);



		//     //backedMesh.mesh_.indices_.AddNextMesh(startVerticesNumber, meshIndices);
		// }

		// //for (Geom::Model2::Mesh& mesh : backedMeshs) {
		// //    model2.meshes_.push_back(std::move(mesh));
		// //}

		//// return model;

	};

}