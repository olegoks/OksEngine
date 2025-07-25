#pragma once
#include <Render\auto_OksEngine.Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <glm/gtc/quaternion.hpp>

//DEBUG
#include <random>

namespace OksEngine
{

	void StartModelAnimation::Update(
		ECS2::Entity::Id entity0id, const Model* model0, const RunModelAnimation* runModelAnimation0,
		const ModelAnimations* modelAnimations0,

		ECS2::Entity::Id entity1id, const Clock* clock1,
		const TimeSinceEngineStart* timeSinceEngineStart1) {

#pragma region Assert
		OS::AssertMessage(
			std::find_if(
				modelAnimations0->animations_.cbegin(),
				modelAnimations0->animations_.cend(),
				[&](const ModelAnimation& modelAnimation) {
					return modelAnimation.name_ == runModelAnimation0->animationName_;
				}) != modelAnimations0->animations_.cend(),
					"Attempt to start animation that doesnt exist.");
#pragma endregion

		CreateComponent<AnimationInProgress>(entity0id, runModelAnimation0->animationName_, 0.0, timeSinceEngineStart1->microseconds_);
		RemoveComponent<RunModelAnimation>(entity0id);

	}

	void ProcessModelAnimation::Update(
		ECS2::Entity::Id entity0id, const Model* model0,
		const ModelAnimations* modelAnimations0, const AnimationInProgress* animationInProgress0,
		const ChildModelNodeEntities* childModelNodeEntities0, ECS2::Entity::Id entity1id, const Clock* clock1,
		const TimeSinceEngineStart* timeSinceEngineStart1) {

		auto it = std::find_if(
			modelAnimations0->animations_.cbegin(),
			modelAnimations0->animations_.cend(),
			[&](const ModelAnimation& modelAnimation) {
				return modelAnimation.name_ == animationInProgress0->animationName_;
			});


#pragma region Assert
		OS::AssertMessage(it != modelAnimations0->animations_.cend(), "");
#pragma endregion

		const ModelAnimation& modelAnimation = *it;

		std::function<void(ECS2::Entity::Id)> processModelNode
			= [&](ECS2::Entity::Id nodeEntityId) {



			if (IsComponentExist<ModelNodeAnimation>(nodeEntityId)) {

				if (IsComponentExist<Name>(nodeEntityId)) {
					if (auto name = GetComponent<Name>(nodeEntityId)->value_ == "Object_55") {
						Common::BreakPointLine();
					}
				}
				auto name = GetComponent<Name>(nodeEntityId);
				auto* modelNodeAnimation = GetComponent<ModelNodeAnimation>(nodeEntityId);

				const std::string& currentAnimationName = animationInProgress0->animationName_;

				const NodeAnimationChannel& nodeChannel = modelNodeAnimation->animationNameToChannel_[currentAnimationName];

				const Common::UInt64 timeSinceAnimationStart = timeSinceEngineStart1->microseconds_ - animationInProgress0->animationStartTimeSinceEngineStart_;

				const double ticksSinceAnimationStart = modelAnimation.ticksPerSecond_ * (timeSinceAnimationStart / 1000000.0);


				auto* localNodeRotation3D = GetComponent<LocalRotation3D>(nodeEntityId);



				auto* localNodePosition3D = GetComponent<LocalPosition3D>(nodeEntityId);

				//TODO: binary search
				for (Common::Index i = 1; i < nodeChannel.position3DValues_.size(); i++) {
					const ChannelPositionKey& previousPosition3DKey = nodeChannel.position3DValues_[i - 1];
					const ChannelPositionKey& currentPosition3DKey = nodeChannel.position3DValues_[i];
					if (ticksSinceAnimationStart > previousPosition3DKey.time_ && ticksSinceAnimationStart < currentPosition3DKey.time_) {

						if (glm::distance(currentPosition3DKey.position3D_, previousPosition3DKey.position3D_) < 10000 * std::numeric_limits<decltype(previousPosition3DKey.position3D_.x)>::epsilon()) {
							break;
						}

						const double intervalTime = currentPosition3DKey.time_ - previousPosition3DKey.time_;
						const double currentTimeInInterval = ticksSinceAnimationStart - previousPosition3DKey.time_;
						//currentTimeInInterval to [0 .. 1].
						float normalizedCurrentTimeInInterval = currentTimeInInterval / intervalTime;
						const glm::vec3 currentPosition3D = glm::mix(previousPosition3DKey.position3D_, currentPosition3DKey.position3D_, normalizedCurrentTimeInInterval);

						if (
							std::isnan(currentPosition3D.x) ||
							std::isnan(currentPosition3D.y) ||
							std::isnan(currentPosition3D.z)) {
							Common::BreakPointLine();
						}

#pragma region Assert
						OS::AssertMessage(
							!std::isnan(currentPosition3D.x) &&
							!std::isnan(currentPosition3D.y) &&
							!std::isnan(currentPosition3D.z), "");
#pragma endregion
						localNodePosition3D->x_ = currentPosition3D.x;
						localNodePosition3D->y_ = currentPosition3D.y;
						localNodePosition3D->z_ = currentPosition3D.z;
						break;
					}
				}

				auto* localNodeScale3D = GetComponent<LocalScale3D>(nodeEntityId);




				for (Common::Index i = 1; i < nodeChannel.rotation3DValues_.size(); i++) {
					const ChannelRotationKey& previousRotation3DKey = nodeChannel.rotation3DValues_[i - 1];
					const ChannelRotationKey& currentRotation3DKey = nodeChannel.rotation3DValues_[i];

					if (ticksSinceAnimationStart > previousRotation3DKey.time_ && ticksSinceAnimationStart < currentRotation3DKey.time_) {
						const double intervalTime = currentRotation3DKey.time_ - previousRotation3DKey.time_;
						const double currentTimeInInterval = ticksSinceAnimationStart - previousRotation3DKey.time_;
						//currentTimeInInterval to [0 .. 1].
						float normalizedCurrentTimeInInterval = currentTimeInInterval / intervalTime;
						const glm::quat currentRotation3D = glm::slerp(previousRotation3DKey.rotation3D_, currentRotation3DKey.rotation3D_, normalizedCurrentTimeInInterval);
						localNodeRotation3D->w_ = currentRotation3D.w;
						localNodeRotation3D->x_ = currentRotation3D.x;
						localNodeRotation3D->y_ = currentRotation3D.y;
						localNodeRotation3D->z_ = currentRotation3D.z;
						break;
					}
				}
			}


			//modelNodeAnimation->animationNameToChannel_

			if (IsComponentExist<ChildModelNodeEntities>(nodeEntityId)) {
				const auto* childModelNodeEntities = GetComponent<ChildModelNodeEntities>(nodeEntityId);
				for (ECS2::Entity::Id childModelNodeEntityId : childModelNodeEntities->childEntityIds_) {
					processModelNode(childModelNodeEntityId);
				}
			}

			};

		const Common::UInt64 timeSinceAnimationStart = timeSinceEngineStart1->microseconds_ - animationInProgress0->animationStartTimeSinceEngineStart_;

		const double ticksSinceAnimationStart = modelAnimation.ticksPerSecond_ * (timeSinceAnimationStart / 1000000.0);

		if (ticksSinceAnimationStart > modelAnimation.durationInTicks_) {
			RemoveComponent<AnimationInProgress>(entity0id);

			//DEBUG
			const Common::Size animationsNumber = modelAnimations0->animations_.size();

			std::random_device rd;
			std::mt19937 gen(rd());

			// Диапазон [a, b]
			Common::Index a = 0, b = animationsNumber - 1;
			std::uniform_int_distribution<> dist(a, b);
			const Common::Index randomAnimationIndex = dist(gen);

			const ModelAnimation& randomModelAnimation = modelAnimations0->animations_[randomAnimationIndex];

			CreateComponent<RunModelAnimation>(entity0id, randomModelAnimation.name_);
			return;
		}

		for (ECS2::Entity::Id childModelNodeEntityId : childModelNodeEntities0->childEntityIds_) {

			processModelNode(childModelNodeEntityId);
		}

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
		const ModelFile* modelFile0,
		const WorldPosition3D* worldPosition3D0,
		const WorldRotation3D* worldRotation3D0,
		const WorldScale3D* worldScale3D0,

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
			aiProcess_ConvertToLeftHanded;

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

		std::map<aiNode*, ECS2::Entity::Id> nodeToEntityId;
		 

		auto createNodeComponents = [this](const aiScene* scene, aiNode* node, ECS2::Entity::Id nodeEntityId) {

			std::vector<ECS2::Entity::Id> meshEntities;

			std::cout << node->mName.C_Str() << std::endl;

			if (node->mNumMeshes > 0) {


				Geom::VertexCloud<Geom::Vertex3f>   vertices;
				DS::Vector<Geom::Normal3f>	        normals;
				//DS::Vector<Geom::Color4b>		    colors;
				DS::Vector<Geom::UV2f>		        uvs;
				Geom::IndexBuffer<Geom::Index16>	indices;

				for (Common::Index i = 0; i < node->mNumMeshes; i++) {
					int meshIndex = node->mMeshes[i];
					aiMesh* mesh = scene->mMeshes[meshIndex];

					std::cout <<"\t" << mesh->mName.C_Str() << std::endl;

					const ECS2::Entity::Id meshEntity
						= CreateEntity();
					CreateComponent<Name>(meshEntity, std::string{ mesh->mName.C_Str() });
					CreateComponent<ModelNodeEntityId>(meshEntity, nodeEntityId);




					meshEntities.push_back(meshEntity);
					aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
					aiString aiTexturePath;
					material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &aiTexturePath);

					const aiTexture* texture = scene->GetEmbeddedTexture(aiTexturePath.C_Str());

					Common::UInt32 textureWidth = texture->mWidth;
					Common::UInt32 textureHeight = texture->mHeight;
					if (textureHeight > 0) {
						CreateComponent<TextureInfo>(meshEntity, aiTexturePath.C_Str());
						CreateComponent<Texture>(
							meshEntity,
							textureWidth, textureHeight,
							std::vector<Geom::Color4b>{
							(Geom::Color4b*)texture->pcData,
								(Geom::Color4b*)texture->pcData + textureWidth * textureHeight});
					}
					else {
						const unsigned char* compressed_data = reinterpret_cast<const unsigned char*>(texture->pcData);

						int width, height, channels;
						// Декодируем сжатые данные (PNG/JPEG)
						unsigned char* pixels = stbi_load_from_memory(
							compressed_data,
							texture->mWidth,  // Размер данных в байтах
							&width, &height, &channels,
							STBI_rgb_alpha    // Формат на выходе: RGBA
						);
						CreateComponent<TextureInfo>(meshEntity, aiTexturePath.C_Str());
						CreateComponent<Texture>(
							meshEntity,
							width, height,
							std::vector<Geom::Color4b>{
							(Geom::Color4b*)pixels,
								(Geom::Color4b*)pixels + width * height});
					}

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
					CreateComponent<Vertices3D>(meshEntity, vertices);
					CreateComponent<Normals>(meshEntity, normals);
					CreateComponent<UVs>(meshEntity, uvs);
					CreateComponent<Indices>(meshEntity, indices);

					vertices.Clear();
					normals.Clear();
					uvs.Clear();
					indices.Clear();
				}


			}
			aiVector3D position3D;
			aiQuaternion rotation3D;
			aiVector3D scale3D;
			aiMatrix4x4 flipYtoZ;
			node->mTransformation.Decompose(scale3D, rotation3D, position3D);

			if (!meshEntities.empty()) {
				CreateComponent<MeshEntities>(nodeEntityId, meshEntities);
			}

			CreateComponent<LocalPosition3D>(nodeEntityId, position3D.x, position3D.y, position3D.z);
			CreateComponent<WorldPosition3D>(nodeEntityId, 0.0f, 0.0f, 0.0f);

			CreateComponent<LocalRotation3D>(nodeEntityId, rotation3D.w, rotation3D.x, rotation3D.y, rotation3D.z);
			CreateComponent<WorldRotation3D>(nodeEntityId, 1.0f, 0.0f, 0.0f, 0.0f);

			CreateComponent<LocalScale3D>(nodeEntityId, scale3D.x, scale3D.y, scale3D.z);
			CreateComponent<WorldScale3D>(nodeEntityId, 0.0f, 0.0f, 0.0f);

			};

		//std::map<std::string, ECS2::Entity::Id> nodeNameToEntityId;

		std::function<ECS2::Entity::Id(const aiScene*, aiNode*)> processChildrenNode = [&](const aiScene* scene, aiNode* node) -> ECS2::Entity::Id {

			const ECS2::Entity::Id nodeEntityId = CreateEntity();
			if (std::string{ node->mName.C_Str() } == "driver_11") {
				Common::BreakPointLine();
			}
			auto createNodeAnimationChannel = [&](ECS2::Entity::Id nodeEntity, aiNode* node, const aiScene* scene) {

				std::map<std::string, NodeAnimationChannel> animationNameToChannel;
				for (Common::Index i = 0; i < scene->mNumAnimations; i++) {
					aiAnimation* animation = scene->mAnimations[i];
					for (Common::Index channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
						aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
						if (nodeAnim->mNodeName == node->mName) {

							NodeAnimationChannel nodeAnimationChannel;
							nodeAnimationChannel.position3DValues_.reserve(nodeAnim->mNumPositionKeys);
							nodeAnimationChannel.rotation3DValues_.reserve(nodeAnim->mNumRotationKeys);
							nodeAnimationChannel.scale3DValues_.reserve(nodeAnim->mNumScalingKeys);

							for (Common::Index positionKeyIndex = 0; positionKeyIndex < nodeAnim->mNumPositionKeys; positionKeyIndex++) {
								aiVectorKey positionKey = nodeAnim->mPositionKeys[positionKeyIndex];
								nodeAnimationChannel.position3DValues_.push_back(
									{ positionKey.mTime, { positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z } }
								);
							}

							for (Common::Index rotationKeyIndex = 0; rotationKeyIndex < nodeAnim->mNumRotationKeys; rotationKeyIndex++) {
								aiQuatKey rotationKey = nodeAnim->mRotationKeys[rotationKeyIndex];
								nodeAnimationChannel.rotation3DValues_.push_back(
									{ rotationKey.mTime, {  rotationKey.mValue.w, rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z } }
								);
							}

							for (Common::Index scaleKeyIndex = 0; scaleKeyIndex < nodeAnim->mNumScalingKeys; scaleKeyIndex++) {
								aiVectorKey scaleKey = nodeAnim->mScalingKeys[scaleKeyIndex];
								nodeAnimationChannel.scale3DValues_.push_back(
									{ scaleKey.mTime, { scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z } }
								);
							}
							if (
								!nodeAnimationChannel.position3DValues_.empty() ||
								!nodeAnimationChannel.rotation3DValues_.empty() ||
								!nodeAnimationChannel.scale3DValues_.empty()) {
								animationNameToChannel[animation->mName.C_Str()] = nodeAnimationChannel;
							}
						}
					}
				}
				if(!animationNameToChannel.empty()) {
					CreateComponent<ModelNodeAnimation>(nodeEntity, animationNameToChannel);
					animationNameToChannel.clear();
				}
				

				};
			//Parse animations.
			if (scene->HasAnimations()) {


				createNodeAnimationChannel(nodeEntityId, node, scene);

			}

			CreateComponent<ModelNode>(nodeEntityId);

			CreateComponent<Name>(nodeEntityId, std::string{ node->mName.C_Str() });

			//Parse meshes.
			createNodeComponents(scene, node, nodeEntityId);
			std::vector<ECS2::Entity::Id> childNodeEntityIds;
			for (Common::Index i = 0; i < node->mNumChildren; i++) {
				aiNode* childrenNode = node->mChildren[i];
				const ECS2::Entity::Id childEntityId = processChildrenNode(scene, childrenNode);
				childNodeEntityIds.push_back(childEntityId);
			}
			if (!childNodeEntityIds.empty()) {
				CreateComponent<ChildModelNodeEntities>(nodeEntityId, childNodeEntityIds);
			}
			return nodeEntityId;
			};

		const ECS2::Entity::Id rootNodeEntityId = processChildrenNode(scene, scene->mRootNode);

		//Create ModelAnimations.
		std::vector<ModelAnimation> modelAnimations;
		modelAnimations.reserve(scene->mNumAnimations);
		for (Common::Index i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* animation = scene->mAnimations[i];
			ModelAnimation modelAnimation{
				animation->mName.C_Str(),
				animation->mDuration,
				animation->mTicksPerSecond
			};
			modelAnimations.push_back(std::move(modelAnimation));
		}
		//for (std::uint32_t i = 0; i < scene->mNumAnimations; i++) {
		//	aiAnimation* anim = scene->mAnimations[i];
		//	std::cout << "Animation: " << anim->mName.C_Str()
		//		<< ", Duration: " << anim->mDuration
		//		<< ", Ticks/sec: " << anim->mTicksPerSecond << std::endl;

		//	for (std::uint32_t k = 0; k < anim->mNumChannels; k++) {
		//		aiNodeAnim* nodeAnim = anim->mChannels[k];
		//		std::cout << "Animation channel name: " << nodeAnim->mNodeName.C_Str() << std::endl;
		//	}

		//	// Обработка каждой костной анимации
		//	for (uint32_t j = 0; j < anim->mNumChannels; j++) {
		//		aiNodeAnim* nodeAnim = anim->mChannels[j];
		//		std::cout << "  Bone: " << nodeAnim->mNodeName.C_Str()
		//			<< ", Pos keys: " << nodeAnim->mNumPositionKeys
		//			<< ", Rot keys: " << nodeAnim->mNumRotationKeys << std::endl;
		//	}


		//}
		CreateComponent<ModelAnimations>(entity0id, std::move(modelAnimations));



		CreateComponent<ChildModelNodeEntities>(entity0id, std::vector{ rootNodeEntityId });
		CreateComponent<Model>(entity0id);
	};


	void CreateRenderPass::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0) {


		auto driver = renderDriver0->driver_;

		std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
		{
			RAL::Driver::RP::AttachmentUsage depthTestAttachment{
				.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
				.initialState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
				.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
				.finalState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite
			};
			attachmentsUsage.push_back(depthTestAttachment);

			RAL::Driver::RP::AttachmentUsage GBufferAttachment{
				.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
				.initialState_ = RAL::Driver::Texture::State::DataIsUndefined,
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
				.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
				.finalState_ = RAL::Driver::Texture::State::DataForColorWrite
			};
			attachmentsUsage.push_back(GBufferAttachment);

		}

		std::vector<RAL::Driver::RP::Subpass> subpasses;
		{
			RAL::Driver::RP::Subpass subpass{
				.colorAttachments_ = { 1 },
				.depthStencilAttachment_ { 0 }// Index of attachment.
			};
			subpasses.push_back(subpass);
		}

		RAL::Driver::RP::CI rpCI{
			.name_ = "RenderPass",
			.attachments_ = attachmentsUsage,
			.subpasses_ = subpasses
		};

		const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

		//DEPTH BUFFER
		RAL::Driver::Texture::CreateInfo1 depthBufferCreateInfo{
			.name_ = "Depth buffer",
			.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
			.size_ = { 2560, 1440 },
			.targetState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
			.targetAccess_ = RAL::Driver::Texture::Access::DepthStencilWrite,
			.targetStages_ = { RAL::Driver::Pipeline::Stage::EarlyFragmentTests, RAL::Driver::Pipeline::Stage::LateFragmentTests },
			.usages_ = { RAL::Driver::Texture::Usage::DepthStencilAttachment },
			.mipLevels_ = 1
		};
		const RAL::Driver::Texture::Id depthBufferId = driver->CreateTexture(depthBufferCreateInfo);

		//RENDER BUFFER
		RAL::Driver::Texture::CreateInfo1 renderBufferCreateInfo{
			.name_ = "Render buffer",
			.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
			.size_ = { 2560, 1440 },
			.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
			.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
			.targetStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
			.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment, RAL::Driver::Texture::Usage::TransferSource },
			.mipLevels_ = 1
		};
		const RAL::Driver::Texture::Id renderBufferId = driver->CreateTexture(renderBufferCreateInfo);


		RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
			.rpId_ = renderPassId,
			.textures_ = { depthBufferId, renderBufferId },
			.size_ = glm::u32vec2{ 2560, 1440 }
		};

		RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = driver->CreateAttachmentSet(attachmentSetCI);

		CreateComponent<RenderPass>(entity0id,
			renderPassId,
			rpAttachmentsSetId,
			std::vector<Common::Id>{ depthBufferId, renderBufferId });
	}


	void CreatePipeline::Update(
		ECS2::Entity::Id entity0id,
		const RenderDriver* renderDriver0,
		const RenderPass* renderPass0,

		ECS2::Entity::Id entity1id,
		const ResourceSystem* resourceSystem1) {

		Resources::ResourceData vertexTextureShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/textured.vert");
		Resources::ResourceData fragmentTextureShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/textured.frag");

		std::string vertexShaderCode{ vertexTextureShaderResource.GetData<Common::Byte>(), vertexTextureShaderResource.GetSize() };
		std::string fragmentShaderCode{ fragmentTextureShaderResource.GetData<Common::Byte>(), fragmentTextureShaderResource.GetSize() };


		RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "TexturedVertexShader",
			.type_ = RAL::Driver::Shader::Type::Vertex,
			.code_ = vertexShaderCode
		};
		auto vertexShader = renderDriver0->driver_->CreateShader(vertexShaderCreateInfo);

		RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "TexturedFragmentShader",
			.type_ = RAL::Driver::Shader::Type::Fragment,
			.code_ = fragmentShaderCode
		};
		auto fragmentShader = renderDriver0->driver_->CreateShader(fragmentShaderCreateInfo);

		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

		RAL::Driver::Shader::Binding::Layout cameraBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
			.stage_ = RAL::Driver::Shader::Stage::VertexShader
		};

		RAL::Driver::Shader::Binding::Layout transformBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
			.stage_ = RAL::Driver::Shader::Stage::VertexShader
		};

		RAL::Driver::Shader::Binding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
			.stage_ = RAL::Driver::Shader::Stage::FragmentShader
		};

		shaderBindings.push_back(cameraBinding);
		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::Pipeline::CI pipelineCI{
			.name_ = "Textured Pipeline",
			.renderPassId_ = renderPass0->rpId_,
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2,
			.indexType_ = RAL::Driver::IndexType::UI16,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::Back,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = true,
			.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less

		};

		const RAL::Driver::Pipeline::Id pipelineId = renderDriver0->driver_->CreatePipeline(pipelineCI);

		CreateComponent<Pipeline>(entity0id, pipelineId);

	}



	void BeginRenderPass::Update(
		ECS2::Entity::Id entity0id,
		RenderDriver* renderDriver0,
		const RenderPass* renderPass0,
		const Pipeline* pipeline0) {

		renderDriver0->driver_->BeginRenderPass(
			renderPass0->rpId_,
			renderPass0->attachmentsSetId_,
			{ 0, 0 },
			{ 2560, 1440 });

	}

	void UpdateLocalPosition3D::Update(
		ECS2::Entity::Id entity0id,
		const Model* model0,
		const ChildModelNodeEntities* childModelNodeEntities0,
		const WorldPosition3D* worldPosition3D0,
		const WorldRotation3D* worldRotation3D0,
		const WorldScale3D* worldScale3D0) {

		std::function<void(ECS2::Entity::Id,
			const glm::fvec3& position3D,
			const glm::quat& rotation3D,
			const glm::fvec3& scale3D)> processModelNode = [&processModelNode, this](
				ECS2::Entity::Id nodeEntityId,
				const glm::fvec3& parentPosition3D,
				const glm::quat& parentRotation3D,
				const glm::fvec3& parentScale3D) {

					if (IsComponentExist<Name>(nodeEntityId)) {
						if (auto name = GetComponent<Name>(nodeEntityId)->value_ == "Object_55") {
							Common::BreakPointLine();
						}
					}

					const auto* localNodeRotation3D = GetComponent<LocalRotation3D>(nodeEntityId);
					auto* worldNodeRotation3D = GetComponent<WorldRotation3D>(nodeEntityId);

					glm::quat worldRotationQuat = parentRotation3D * glm::quat(localNodeRotation3D->w_, localNodeRotation3D->x_, localNodeRotation3D->y_, localNodeRotation3D->z_);

					worldNodeRotation3D->w_ = worldRotationQuat.w;
					worldNodeRotation3D->x_ = worldRotationQuat.x;
					worldNodeRotation3D->y_ = worldRotationQuat.y;
					worldNodeRotation3D->z_ = worldRotationQuat.z;

					const auto* localNodePosition3D = GetComponent<LocalPosition3D>(nodeEntityId);
					auto* worldNodePosition3D = GetComponent<WorldPosition3D>(nodeEntityId);

					{
						glm::vec3 worldNodePosition3DVec = parentPosition3D + parentRotation3D * (parentScale3D * glm::vec3{ localNodePosition3D->x_, localNodePosition3D->y_, localNodePosition3D->z_ });


#pragma region Assert
						OS::AssertMessage(
							!std::isnan(worldNodePosition3DVec.x) &&
							!std::isnan(worldNodePosition3DVec.y) &&
							!std::isnan(worldNodePosition3DVec.z), "");
#pragma endregion


						worldNodePosition3D->x_ = worldNodePosition3DVec.x;
						worldNodePosition3D->y_ = worldNodePosition3DVec.y;
						worldNodePosition3D->z_ = worldNodePosition3DVec.z;
					}

					const auto* localNodeScale3D = GetComponent<LocalScale3D>(nodeEntityId);
					auto* worldNodeScale3D = GetComponent<WorldScale3D>(nodeEntityId);

					worldNodeScale3D->x_ = parentScale3D.x * localNodeScale3D->x_;
					worldNodeScale3D->y_ = parentScale3D.y * localNodeScale3D->y_;
					worldNodeScale3D->z_ = parentScale3D.z * localNodeScale3D->z_;


					if (IsComponentExist<ChildModelNodeEntities>(nodeEntityId)) {
						const auto* childModelNodeEntities = GetComponent<ChildModelNodeEntities>(nodeEntityId);
						for (ECS2::Entity::Id childModelNodeEntityId : childModelNodeEntities->childEntityIds_) {
							const glm::fvec3 currentNodePosition3D = { worldNodePosition3D->x_, worldNodePosition3D->y_, worldNodePosition3D->z_ };
							const glm::quat currentNodeRotation3D = { worldNodeRotation3D->w_, worldNodeRotation3D->x_, worldNodeRotation3D->y_, worldNodeRotation3D->z_ };
							const glm::fvec3 currentNodeScale3D = { worldNodeScale3D->x_, worldNodeScale3D->y_, worldNodeScale3D->z_ };
							processModelNode(childModelNodeEntityId, currentNodePosition3D, currentNodeRotation3D, currentNodeScale3D);
						}
					}

			};



		for (ECS2::Entity::Id childModelNodeEntityId : childModelNodeEntities0->childEntityIds_) {

			const glm::fvec3 position3D = { worldPosition3D0->x_, worldPosition3D0->y_, worldPosition3D0->z_ };
			const glm::quat rotation3D = { worldRotation3D0->w_, worldRotation3D0->x_, worldRotation3D0->y_, worldRotation3D0->z_ };
			const glm::fvec3 scale3D = { worldScale3D0->x_, worldScale3D0->y_, worldScale3D0->z_ };
			processModelNode(childModelNodeEntityId, position3D, rotation3D, scale3D);
		}


	}

	void AddModelToRender::Update(
		ECS2::Entity::Id entity0id,
		const Camera* camera0,
		const Active* active0,
		const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer0,
		const CameraTransformResource* cameraTransformResource0,

		ECS2::Entity::Id entity1id,
		const Indices* indices1,
		const DriverIndexBuffer* driverIndexBuffer1,
		const DriverVertexBuffer* driverVertexBuffer1,
		const TextureResource* textureResource1,
		const ModelNodeEntityId* modelNodeEntityId1,

		ECS2::Entity::Id entity2id,
		RenderDriver* renderDriver2,
		const RenderPass* renderPass2,
		const Pipeline* pipeline2) {


		auto driver = renderDriver2->driver_;

		driver->SetViewport(0, 0, 2560, 1440);
		driver->SetScissor(0, 0, 2560, 1440);
		driver->BindPipeline(pipeline2->id_);
		driver->BindVertexBuffer(driverVertexBuffer1->id_, 0);
		driver->BindIndexBuffer(driverIndexBuffer1->id_, 0);

		const ECS2::Entity::Id nodeEntityId = modelNodeEntityId1->nodeEntityId_;


#pragma region Assert
		OS::AssertMessage(IsComponentExist<Transform3DResource>(nodeEntityId), "");
		const auto* transform3DResource = GetComponent<Transform3DResource>(nodeEntityId);
#pragma endregion


		driver->Bind(pipeline2->id_,
			{
				cameraTransformResource0->id_,
				transform3DResource->id_,
				textureResource1->id_
			});
		driver->DrawIndexed(indices1->indices_.GetIndicesNumber());


	}

	void EndRenderPass::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0, const RenderPass* renderPass0,
		const Pipeline* pipeline0) {

		auto driver = renderDriver0->driver_;

		driver->EndSubpass();
		driver->EndRenderPass();

		//driver->Show(renderPass0->textureIds_[1]);

	}
}