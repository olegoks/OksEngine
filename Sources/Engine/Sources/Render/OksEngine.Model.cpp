#pragma once
#include <Render\auto_OksEngine.Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

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

	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& aiMat) {
		glm::mat4 glmMat;

		glmMat[0][0] = aiMat.a1; glmMat[0][1] = aiMat.b1; glmMat[0][2] = aiMat.c1; glmMat[0][3] = aiMat.d1;
		glmMat[1][0] = aiMat.a2; glmMat[1][1] = aiMat.b2; glmMat[1][2] = aiMat.c2; glmMat[1][3] = aiMat.d2;
		glmMat[2][0] = aiMat.a3; glmMat[2][1] = aiMat.b3; glmMat[2][2] = aiMat.c3; glmMat[2][3] = aiMat.d3;
		glmMat[3][0] = aiMat.a4; glmMat[3][1] = aiMat.b4; glmMat[3][2] = aiMat.c4; glmMat[3][3] = aiMat.d4;

		return glmMat;
	}

	void CreateModel::Update(
		ECS2::Entity::Id entity0id, 
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
			aiProcess_OptimizeMeshes |
			aiProcess_PreTransformVertices;

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

		auto createNodeComponents = [this](const aiScene* scene, aiNode* node, ECS2::Entity::Id nodeEntityId) {

			std::vector<ECS2::Entity::Id> meshEntities;

			if (node->mNumMeshes > 0) {


				Geom::VertexCloud<Geom::Vertex3f>   vertices;
				DS::Vector<Geom::Normal3f>	        normals;
				//DS::Vector<Geom::Color4b>		    colors;
				DS::Vector<Geom::UV2f>		        uvs;
				Geom::IndexBuffer<Geom::Index16>	indices;

				for (Common::Index i = 0; i < node->mNumMeshes; i++) {
					int meshIndex = node->mMeshes[i];
					aiMesh* mesh = scene->mMeshes[meshIndex];



					const ECS2::Entity::Id meshEntity = CreateEntity();
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
			glm::mat4 transform{ aiMatrix4x4ToGlm(node->mTransformation) };
			glm::vec3 translation{};
			glm::quat rotation;
			glm::vec3 scale;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform, scale, rotation, translation, skew, perspective);
			if (!meshEntities.empty()) {
				CreateComponent<MeshEntities>(nodeEntityId, meshEntities);
			}

			CreateComponent<Position3D>(nodeEntityId, translation.x, translation.y, translation.z);
			CreateComponent<Rotation3D>(nodeEntityId, rotation.w, rotation.x, rotation.y, rotation.z);


			};

		std::function<ECS2::Entity::Id(const aiScene*, aiNode*)> processChildrenNode
			= [this, &processChildrenNode, &createNodeComponents](const aiScene* scene, aiNode* node) -> ECS2::Entity::Id {

			const ECS2::Entity::Id nodeEntityId = CreateEntity();
			CreateComponent<ModelNode>(nodeEntityId);
			CreateComponent<Name>(nodeEntityId, std::string{ node->mName.C_Str() });
			createNodeComponents(scene, node, nodeEntityId);
			std::vector<ECS2::Entity::Id> childNodeEntityIds;
			for (Common::Index i = 0; i < node->mNumChildren; i++) {
				aiNode* childrenNode = node->mChildren[i];
				const ECS2::Entity::Id childEntityId = processChildrenNode(scene, childrenNode);
			}
			if (!childNodeEntityIds.empty()) {
				CreateComponent<ChildModelNodeEntities>(nodeEntityId, childNodeEntityIds);
			}
			return nodeEntityId;
			};


		createNodeComponents(scene, scene->mRootNode, entity0id);
		std::vector<ECS2::Entity::Id> childNodeEntityIds;
		for (Common::Index i = 0; i < scene->mRootNode->mNumChildren; i++) {
			aiNode* childrenNode = scene->mRootNode->mChildren[i];
			const ECS2::Entity::Id childEntityId = processChildrenNode(scene, childrenNode);
			childNodeEntityIds.push_back(childEntityId);
		}
		if (!childNodeEntityIds.empty()) {
			CreateComponent<ChildModelNodeEntities>(entity0id, childNodeEntityIds);
		}
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


	void CreatePipeline::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0, const RenderPass* renderPass0,
		ECS2::Entity::Id entity1id, const ResourceSystem* resourceSystem1) {

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