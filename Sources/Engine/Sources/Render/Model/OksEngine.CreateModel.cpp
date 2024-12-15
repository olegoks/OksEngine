
#include <Render/Model/OksEngine.CreateModel.hpp>

#include <Animation/OksEngine.Animation.Components.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#pragma optimize("", off)

namespace OksEngine {

	void CreateModelEntityFromObjMtl::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		//auto* position = world->GetComponent<Position>(entityId);
		//auto* rotation = world->GetComponent<Rotation>(entityId);
		//auto* objEntity = world->GetComponent<ObjEntity>(entityId);
		//auto* mtlEntity = world->GetComponent<MtlEntity>(entityId);
		//auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);

		//ECS::Entity::Id modelEntityid = world->CreateEntity();
		//std::vector<ECS::Entity::Id> meshsEntitiesIds;

		//auto* objName = world->GetComponent<Name>(objEntity->id_);
		//auto* objData = world->GetComponent<BinaryData>(objEntity->id_);

		//auto* mtlName = world->GetComponent<Name>(mtlEntity->id_);
		//auto* mtlData = world->GetComponent<BinaryData>(mtlEntity->id_);

		//Geom::Model2 model2 = Geom::ParseObjMtlModelBaked(
		//	objName->value_, 
		//	std::string{ objData->data_.data(), objData->data_.size() }, 
		//	mtlName->value_,
		//	std::string{ mtlData->data_.data(), mtlData->data_.size() } );
		//OS::Assert(model2.meshes_.size() == 1);
		//for (auto& mesh : model2.meshes_) {
		//	//ECS::Entity::Id meshEntityId = world->CreateEntity();
		//	//meshsEntitiesIds.push_back(meshEntityId);
		//	world->CreateComponent<Mesh2>(entityId);
		//	//world->CreateComponent<Position>(meshEntityId, position->GetX(), position->GetY(), position->GetZ());
		//	//world->CreateComponent<Rotation>(meshEntityId, rotation->GetRotationVector(), rotation->GetAngle());
		//	if (mesh.textureName_ != "") {
		//		world->CreateComponent<TextureInfo>(entityId, mesh.textureName_);
		//	}
		//	
		//	OS::Assert(mesh.mesh_.vertices_.GetVerticesNumber() != 0);
		//	world->CreateComponent<Vertices3D>(entityId, mesh.vertices_);

		//	OS::Assert(mesh.indices_.GetIndicesNumber() != 0);
		//	world->CreateComponent<Indices>(entityId, mesh.indices_);

		//	if (mesh.colors_.GetSize() != 0) {
		//		world->CreateComponent<Colors>(entityId, mesh.colors_);
		//	}

		//	if (mesh.normals_.GetSize() != 0) {
		//		world->CreateComponent<Normals>(entityId, mesh.normals_);
		//	}

		//	if (mesh.uvs_.GetSize() != 0) {
		//		world->CreateComponent<UVs>(entityId, mesh.uvs_);
		//	}

		//}

		//world->CreateComponent<ChildEntities>(modelEntityid, meshsEntitiesIds);
		//world->CreateComponent<ModelEntity>(entityId, modelEntityid);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromObjMtl::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<ObjEntity>()
			.Include<MtlEntity>()
			.Exclude<Mesh2>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateModelEntityFromObjMtl::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromObjMtl>().GetId();
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

	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& aiMat) {
		glm::mat4 glmMat;

		glmMat[0][0] = aiMat.a1; glmMat[0][1] = aiMat.b1; glmMat[0][2] = aiMat.c1; glmMat[0][3] = aiMat.d1;
		glmMat[1][0] = aiMat.a2; glmMat[1][1] = aiMat.b2; glmMat[1][2] = aiMat.c2; glmMat[1][3] = aiMat.d2;
		glmMat[2][0] = aiMat.a3; glmMat[2][1] = aiMat.b3; glmMat[2][2] = aiMat.c3; glmMat[2][3] = aiMat.d3;
		glmMat[3][0] = aiMat.a4; glmMat[3][1] = aiMat.b4; glmMat[3][2] = aiMat.c4; glmMat[3][3] = aiMat.d4;

		return glmMat;
	}

	glm::vec3 getPositionFromMatrix(const glm::mat4& transformMatrix) {
		// The translation components are located in the last column of the matrix
		return glm::vec3(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);
	}

	void ProcessMesh(ECS::World* world, ECS::Entity::Id meshEntity, const aiScene* scene, aiMesh* mesh) {

		//Process texture.
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//Process texture
		const int texturesNumber = material->GetTextureCount(aiTextureType_DIFFUSE);
		OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");

		//Process mesh only with texture.
		if (texturesNumber != 1) return;

		aiString aiTexturePath;
		const aiReturn result = material->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
		std::string textureName = aiTexturePath.C_Str();
		const Common::Index slashIndex = textureName.rfind('\\');
		if (slashIndex != -1) {
			textureName = textureName.substr(slashIndex + 1);
		}
		OS::AssertMessage(textureName != "", "Invalid Texture name.");

		world->CreateComponent<TextureInfo>(meshEntity, textureName);

		world->CreateComponent<Vertices3D>(meshEntity, (const Geom::Vertex3f*)mesh->mVertices, (Common::Size) mesh->mNumVertices);
		world->CreateComponent<Normals>(meshEntity, (const Geom::Normal3f*)mesh->mNormals, mesh->mNumVertices);
		
		DS::Vector<Geom::UV2f> uvs;
		uvs.Reserve(mesh->mNumVertices);

		for (unsigned j = 0; j < mesh->mNumVertices; j++) {
			uvs.PushBack(Geom::UV2f{ mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y });
		}

		Geom::IndexBuffer meshIndices;
		meshIndices.Reserve(mesh->mNumFaces * 3);
		OS::AssertMessage(mesh->mNumFaces * 3 < Common::Limits<Common::UInt16>::Max(), "Number of indices is more than limit.");
		for (unsigned j = 0; j < mesh->mNumFaces; j++) {
			meshIndices.Add(mesh->mFaces[j].mIndices[0]);
			meshIndices.Add(mesh->mFaces[j].mIndices[1]);
			meshIndices.Add(mesh->mFaces[j].mIndices[2]);
		}

		OS::Assert(meshIndices.GetIndicesNumber() != 0);
		world->CreateComponent<Indices>(meshEntity, meshIndices);

		if (uvs.GetSize() != 0) {
			world->CreateComponent<UVs>(meshEntity, uvs);
		}


	}

	void ProcessAnimation(ECS::World* world, ECS::Entity::Id nodeEntity, const aiScene* scene, aiNode* node) {

	}

	void ProcessNode(ECS::World* world, ECS::Entity::Id nodeEntity, const aiScene* scene, aiNode* node) {

		world->CreateComponent<Name>(nodeEntity, node->mName.C_Str());
		world->CreateComponent<Node>(nodeEntity);

		glm::mat4 transform{ aiMatrix4x4ToGlm(node->mTransformation) };
		glm::vec3 translation{};
		glm::quat rotation;
		glm::vec3 scale;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);

		world->CreateComponent<Position>(nodeEntity, translation);
		world->CreateComponent<Rotation>(nodeEntity, rotation);

		//Process meshes.
		std::vector<ECS::Entity::Id> meshEntityIds;
		meshEntityIds.reserve(node->mNumMeshes);
		for (unsigned int meshIndexIndex = 0; meshIndexIndex < node->mNumMeshes; meshIndexIndex++) {
			const unsigned int meshIndex = node->mMeshes[meshIndexIndex];
			ECS::Entity::Id meshEntityId = world->CreateEntity();
			ProcessMesh(world, meshEntityId, scene, scene->mMeshes[meshIndex]);
			meshEntityIds.push_back(meshEntityId);
		}
		world->CreateComponent<Meshes>(nodeEntity, meshEntityIds);

		OS::AssertMessage(scene->mNumAnimations <=1, "Multiple animationes are not supperted yet.");
		//Process animation.
		for (unsigned int animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++) {
			const aiAnimation* animation = scene->mAnimations[animationIndex];
			for (unsigned int nodeChannelIndex = 0; nodeChannelIndex < animation->mNumChannels; nodeChannelIndex++) {
				const aiNodeAnim* nodeAnim = animation->mChannels[nodeChannelIndex];
				if (nodeAnim->mNodeName == node->mName) {
					Animation::CreateInfo animCI;
					//Process node animation.
					
					animCI.name_ = animation->mName.C_Str();

					animCI.duration_ = animation->mDuration;
					animCI.ticksPerSecond_ = animation->mTicksPerSecond;

					//Process position keys.
					for (unsigned int positionKeyIndex = 0; positionKeyIndex < nodeAnim->mNumPositionKeys; positionKeyIndex++) {
						const aiVectorKey aiPositionKey = nodeAnim->mPositionKeys[positionKeyIndex];
						Animation::PositionKey positionKey{
							.time_ = aiPositionKey.mTime,
							.value_ = { aiPositionKey.mValue.x, aiPositionKey.mValue.y, aiPositionKey.mValue.z }
						};
						animCI.positionKeys_.push_back(positionKey);
					}

					//Process rotation keys.
					for (unsigned int rotationKeyIndex = 0; rotationKeyIndex < nodeAnim->mNumRotationKeys; rotationKeyIndex++) {
						const aiQuatKey aiQuatKey = nodeAnim->mRotationKeys[rotationKeyIndex];
						Animation::RotationKey rotationKey{
							.time_ = aiQuatKey.mTime,
							.value_ = { aiQuatKey.mValue.w, aiQuatKey.mValue.x, aiQuatKey.mValue.y, aiQuatKey.mValue.z }
						};
						animCI.rotationKeys_.push_back(rotationKey);
					}
					
					//Process scale keys.
					for (unsigned int scaleKeyIndex = 0; scaleKeyIndex < nodeAnim->mNumRotationKeys; scaleKeyIndex++) {
						const aiVectorKey aiScaleKey = nodeAnim->mScalingKeys[scaleKeyIndex];
						Animation::ScalingKey scalingKey{
							.time_ = aiScaleKey.mTime,
							.value_ = { aiScaleKey.mValue.x, aiScaleKey.mValue.y, aiScaleKey.mValue.z }
						};
						animCI.scalingKeys_.push_back(scalingKey);
					}

					world->CreateComponent<Animation>(nodeEntity, animCI);
				}
			}
		}


		//Process child entities.
		if (node->mNumChildren > 0) {

			std::vector<ECS::Entity::Id> childEntityIds;
			childEntityIds.reserve(node->mNumChildren);

			for (unsigned int childNodeIndex = 0; childNodeIndex < node->mNumChildren; childNodeIndex++) {
				aiNode* childNode = node->mChildren[childNodeIndex];
				ECS::Entity::Id childNodeEntity = world->CreateEntity();
				ProcessNode(world, childNodeEntity, scene, childNode);
				childEntityIds.push_back(childNodeEntity);
			}

			world->CreateComponent<ChildEntities>(nodeEntity, childEntityIds);
		}
	}

	void CreateModelEntityFromFbx::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {


		auto* position = world->GetComponent<Position>(entityId);
		auto* rotation = world->GetComponent<Rotation>(entityId);
		auto* fbxEntity = world->GetComponent<FbxEntity>(entityId);
		auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);

		ECS::Entity::Id modelEntityid = world->CreateEntity();

		auto* fbxName = world->GetComponent<Name>(fbxEntity->id_);
		auto* fbxData = world->GetComponent<BinaryData>(fbxEntity->id_);

		Assimp::Importer importer;

		importer.SetIOHandler(new FbxIOSystem{ fbxName->value_, std::string{ fbxData->data_.data(), fbxData->data_.size() } });

		const aiScene* scene = importer.ReadFile(fbxName->value_,
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
		//Geom::Model2 model2;

		//auto parseAiAnimation = [](const aiAnimation* aiAnim)->std::shared_ptr<Model2::Animation> {
		//	const aiAnimation* animation = aiAnim;

		//	auto animationPtr = std::make_shared<Model2::Animation>();
		//	animationPtr->name_ = animation->mName.C_Str();
		//	animationPtr->ticksNumber_ = animation->mDuration;
		//	animationPtr->ticksPerSecond_ = animation->mTicksPerSecond;
		//	for (Common::Index j = 0; j < animation->mNumChannels; j++) {
		//		for (Common::Index ti = 0; ti < animation->mChannels[j]->mNumPositionKeys; ti++) {
		//			const aiVector3D& aiPosition = animation->mChannels[j]->mPositionKeys[ti].mValue;
		//			const aiQuaternion& aiRotation = animation->mChannels[j]->mRotationKeys[ti].mValue;
		//			Model2::Animation::StateInfo state{
		//				.time_ = animation->mChannels[j]->mPositionKeys[ti].mTime,
		//				.position_ = glm::vec3{ aiPosition.x ,aiPosition.y, aiPosition.z },
		//				.rotation_ = glm::quat::wxyz(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z)
		//			};

		//			animationPtr->states_.push_back(state);
		//		}
		//	}
		//	return animationPtr;
		//	};

		//if (scene->HasAnimations()) {
		//	//OS::AssertMessage(scene->mNumAnimations <= 1, "More than one animation is not supported.");
		//	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {

		//	}
		//}


		OS::AssertMessage(scene->mRootNode != nullptr, "Empty FBX model file.");
		ECS::Entity::Id rootNodeId = world->CreateEntity();
		ProcessNode(world, rootNodeId, scene, scene->mRootNode);
		world->CreateComponent<ModelEntity>(entityId, rootNodeId);




		//Common::UInt64 previousMeshIndicesNumber = 0;
		////std::vector<Model2::Mesh> backedMeshs;
		//for (unsigned i = 0; i < scene->mNumMeshes; i++) {

		//	const aiMesh* aimesh = scene->mMeshes[i];
		//	const auto materialPtr = scene->mMaterials[aimesh->mMaterialIndex];

		//	const int texturesNumber = materialPtr->GetTextureCount(aiTextureType_DIFFUSE);
		//	OS::AssertMessage(texturesNumber <= 1, "Mesh has more than 1 texture.");


		//	//Process mesh only with texture.
		//	if (texturesNumber != 1) continue;

		//	aiString aiTexturePath;
		//	const aiReturn result = materialPtr->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath);
		//	OS::AssertMessage(result == AI_SUCCESS, "Error while getting texture.");
		//	std::string textureName = aiTexturePath.C_Str();
		//	const Common::Index slashIndex = textureName.rfind('\\');
		//	if (slashIndex != -1) {
		//		textureName = textureName.substr(slashIndex + 1);
		//	}

		//	backedMesh.mesh_.textureName_ = textureName;
		//	const Common::Size startVerticesNumber = backedMesh.mesh_.vertices_.GetVerticesNumber();
		//	backedMesh.mesh_.vertices_.Reserve(aimesh->mNumVertices);
		//	backedMesh.mesh_.normals_.Reserve(aimesh->mNumVertices);
		//	backedMesh.mesh_.uvs_.Reserve(aimesh->mNumVertices);
		//	for (unsigned j = 0; j < aimesh->mNumVertices; j++) {
		//		backedMesh.mesh_.vertices_.Add(Vertex3f{ aimesh->mVertices[j].x,
		//											aimesh->mVertices[j].y,
		//											aimesh->mVertices[j].z });
		//		backedMesh.mesh_.normals_.PushBack(Normal3f{ aimesh->mVertices[j].x,
		//												aimesh->mVertices[j].y,
		//												aimesh->mVertices[j].z });
		//		backedMesh.mesh_.uvs_.PushBack(UV2f{ aimesh->mTextureCoords[0][j].x,
		//										aimesh->mTextureCoords[0][j].y });
		//	}
		//	Geom::IndexBuffer meshIndices;
		//	meshIndices.Reserve(aimesh->mNumFaces * 3);
		//	for (unsigned j = 0; j < aimesh->mNumFaces; j++) {
		//		meshIndices.Add(aimesh->mFaces[j].mIndices[0]);
		//		meshIndices.Add(aimesh->mFaces[j].mIndices[1]);
		//		meshIndices.Add(aimesh->mFaces[j].mIndices[2]);
		//	}

		//	backedMesh.mesh_.indices_.AddNextMesh(startVerticesNumber, meshIndices);
		//}

		//for (Geom::Model2::Mesh& mesh : backedMeshs) {
		//	model2.meshes_.push_back(std::move(mesh));
		//}

		//return model2;


		//auto* position = world->GetComponent<Position>(entityId);
		//auto* rotation = world->GetComponent<Rotation>(entityId);
		//auto* fbxEntity = world->GetComponent<FbxEntity>(entityId);
		//auto* geomFileEntity = world->GetComponent<GeometryDescriptionFileEntity>(entityId);

		//ECS::Entity::Id modelEntityid = world->CreateEntity();

		//auto* fbxName = world->GetComponent<Name>(fbxEntity->id_);
		//auto* fbxData = world->GetComponent<BinaryData>(fbxEntity->id_);

		//Geom::Model2 model2 = Geom::ParseFbxModelBaked(
		//	fbxName->value_,
		//	std::string{ fbxData->data_.data(), fbxData->data_.size() });

		////if (!model2.animation_->states_.empty()) {
		////	std::vector<Animation::StateInfo> states;
		////	for (Geom::Model2::Animation::StateInfo& state : model2.animation_->states_) {
		////		states.push_back({ state.time_, state.position_, state.rotation_ });
		////	}
		////	world->CreateComponent<Animation>(entityId, model2.animation_->ticksNumber_, model2.animation_->ticksPerSecond_, states);
		////}



		//std::vector<ECS::Entity::Id> meshesEntitiesIds;
		//for (auto& mesh : model2.meshes_) {
		//	ECS::Entity::Id meshEntityId = world->CreateEntity();
		//	meshesEntitiesIds.push_back(meshEntityId);
		//	world->CreateComponent<Mesh2>(meshEntityId);
		//	world->CreateComponent<Position>(meshEntityId, position->GetX(), position->GetY(), position->GetZ());
		//	world->CreateComponent<Rotation>(meshEntityId, rotation->GetRotationVector(), rotation->GetAngle());
		//	if (mesh.mesh_.textureName_ != "") {
		//		world->CreateComponent<TextureInfo>(meshEntityId, mesh.mesh_.textureName_);
		//	}

		//	if (mesh.animation_ != nullptr) {
		//		std::vector<Animation::StateInfo> states;
		//		for (Geom::Model2::Animation::StateInfo& state : mesh.animation_->states_) {
		//			states.push_back({ state.time_, state.position_, state.rotation_ });
		//		}
		//		world->CreateComponent<Animation>(meshEntityId, mesh.animation_->ticksNumber_, mesh.animation_->ticksPerSecond_, states);
		//	}
		//	OS::Assert(mesh.mesh_.vertices_.GetVerticesNumber() != 0);
		//	world->CreateComponent<Vertices3D>(meshEntityId, mesh.mesh_.vertices_);

		//	OS::Assert(mesh.mesh_.indices_.GetIndicesNumber() != 0);
		//	world->CreateComponent<Indices>(meshEntityId, mesh.mesh_.indices_);

		//	if (mesh.mesh_.colors_.GetSize() != 0) {
		//		world->CreateComponent<Colors>(meshEntityId, mesh.mesh_.colors_);
		//	}

		//	if (mesh.mesh_.normals_.GetSize() != 0) {
		//		world->CreateComponent<Normals>(meshEntityId, mesh.mesh_.normals_);
		//	}

		//	if (mesh.mesh_.uvs_.GetSize() != 0) {
		//		world->CreateComponent<UVs>(meshEntityId, mesh.mesh_.uvs_);
		//	}

		//}

		//world->CreateComponent<ChildEntities>(modelEntityid, meshesEntitiesIds);
		//world->CreateComponent<ModelEntity>(entityId, modelEntityid);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateModelEntityFromFbx::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<FbxEntity>()
			.Exclude<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateModelEntityFromFbx::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateModelEntityFromFbx>().GetId();
	}


}

#pragma optimize("", on)
