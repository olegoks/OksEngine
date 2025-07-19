#pragma once
#include <Debug/auto_OksEngine.ImGuiRender.hpp>

#include <Debug/OksEngine.ImGuiTransform.hpp>

#include <Common.StringLiterals.hpp>
#include <Geometry.IndexBuffer.hpp>


namespace OksEngine
{
	void CreateImGuiPipeline::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImGuiRenderPass* imGuiRenderPass0,
		ECS2::Entity::Id entity1id, const RenderDriver* renderDriver1, ECS2::Entity::Id entity2id,
		const ResourceSystem* resourceSystem2) {

		auto driver = renderDriver1->driver_;

		Resources::ResourceData imguiVertexShaderResource
			= resourceSystem2->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/imgui.vert");
		Resources::ResourceData imguiFragmentShaderResource
			= resourceSystem2->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/imgui.frag");

		std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
		std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };


		RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "ImGuiVertexShader",
			.type_ = RAL::Driver::Shader::Type::Vertex,
			.code_ = imguiVertexShader
		};
		auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

		RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "ImGuiFragmentShader",
			.type_ = RAL::Driver::Shader::Type::Fragment,
			.code_ = imguiFragmentShader
		};
		auto fragmentShader = driver->CreateShader(fragmentShaderCreateInfo);

		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

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

		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::Pipeline::CI pipelineCI{
			.name_ = "ImGui Pipeline",
			.renderPassId_ = imGuiRenderPass0->rpId_,
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::None,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = true,
			.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always

		};

		const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

		CreateComponent<ImGuiPipeline>(entity0id, pipelineId);

	};

	void CreateImGuiRenderPass::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, ECS2::Entity::Id entity1id,
		const RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
		{
			RAL::Driver::RP::AttachmentUsage attachment{
				.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
				.initialState_ = RAL::Driver::Texture::State::DataIsUndefined,
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
				.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
				.finalState_ = RAL::Driver::Texture::State::DataForColorWrite
			};
			attachmentsUsage.push_back(attachment);
		}

		std::vector<RAL::Driver::RP::Subpass> subpasses;
		{
			RAL::Driver::RP::Subpass subpass{
				.colorAttachments_ = { 0 } // Index of attachment.
			};
			subpasses.push_back(subpass);
		}

		RAL::Driver::RP::CI rpCI{
			.name_ = "ImGuiRenderPass",
			.attachments_ = attachmentsUsage,
			.subpasses_ = subpasses
		};

		const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

		RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
			.name_ = "ImGui render buffer",
			.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
			.size_ = { 1920, 1080 },
			.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
			.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
			.targetStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
			.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment, RAL::Driver::Texture::Usage::TransferSource },
			.mipLevels_ = 1
			
		};
		const RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);

		RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
			.rpId_ = renderPassId,
			.textures_ = { textureId },
			.size_ = glm::u32vec2{ 1920, 1080 }
		};

		RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = driver->CreateAttachmentSet(attachmentSetCI);

		CreateComponent<ImGuiRenderPass>(entity0id,
			renderPassId,
			rpAttachmentsSetId,
			std::vector<Common::Id>{ textureId });


	}

	void BeginImGuiRenderPass::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImGuiRenderPass* imGuiRenderPass0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		driver->BeginRenderPass(imGuiRenderPass0->rpId_, imGuiRenderPass0->attachmentsSetId_, { 0, 0 }, { 1920, 1080 });
		driver->BeginSubpass();

	}


	void AddImGuiMeshToRender::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const ImGuiPipeline* imGuiPipeline0,
		const ImGuiRenderPass* imGuiRenderPass0,
		const MainMenuBar* mainMenuBar0,
		const Transform2DResource* transform2DResource0,
		const TextureResource* textureResource0,
		const ImGuiDriverIndexBuffer* imGuiDriverIndexBuffer0,
		const ImGuiDriverVertexBuffer* imGuiDriverVertexBuffer0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		driver->SetViewport(0, 0, 1920, 1080);
		driver->SetScissor(0, 0, 1920, 1080);
		driver->BindPipeline(imGuiPipeline0->id_);
		driver->BindVertexBuffer(imGuiDriverVertexBuffer0->id_, 0);
		driver->BindIndexBuffer(imGuiDriverIndexBuffer0->id_, 0);
		driver->Bind(imGuiPipeline0->id_,
			{
				transform2DResource0->id_,
				textureResource0->id_
			});
		driver->DrawIndexed(imGuiDriverIndexBuffer0->size_ / sizeof(Common::UInt32));

 	}


	void EndImGuiRenderPass::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImGuiRenderPass* imGuiRenderPass0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		driver->EndSubpass();
		driver->EndRenderPass();

		driver->Show(imGuiRenderPass0->textureIds_[0]);

	}

	void CreateImGuiDriverVertexBuffer::Update(ECS2::Entity::Id entity1Id,
		const ImGuiState* imGuiState,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {


		auto driver = renderDriver->driver_;

		const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = capacity,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		CreateComponent<ImGuiDriverVertexBuffer>(entity1Id, VBId, capacity, 0);
	};


	void CreateImGuiDriverIndexBuffer::Update(ECS2::Entity::Id entity1Id,
		const ImGuiState* imGuiState,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {


		auto driver = renderDriver->driver_;

		using namespace Common;

		constexpr Common::UInt64 capacity = 30_MB;

		RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
			.indicesNumber_ = capacity,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.type_ = RAL::Driver::IndexBuffer::Type::Const
		};
		const RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);

		CreateComponent<ImGuiDriverIndexBuffer>(entity1Id, IBId, capacity, 0);

	};

	void UpdateImGuiDriverTransform2D::Update(
		ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
		const DriverTransform2D* driverTransform2D, ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr) return;
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;


		const glm::vec2 scale{
			2.0f / draw_data->DisplaySize.x,
			2.0f / draw_data->DisplaySize.y
		};

		const glm::vec2 translate{
			-1.0f - draw_data->DisplayPos.x * scale[0],
			-1.0f - draw_data->DisplayPos.y * scale[1]
		};

		ImGuiTransform transform{
			scale,
			translate
		};

		auto driver = renderDriver->driver_;

		driver->FillUniformBuffer(driverTransform2D->id_, &transform);

	};


	void CreateImGuiDriverTransform2D::Update(
		ECS2::Entity::Id entity1Id,
		const ImGuiState* imGuiState,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		auto driver = renderDriver->driver_;

		const RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(ImGuiTransform),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		const RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);

		CreateComponent<DriverTransform2D>(entity1Id, ubId);

	};

	void CreateImGuiAtlasTexture::Update(ECS2::Entity::Id entity0id, ImGuiState* imGuiState0) {

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Build();
		if (!io.Fonts->IsBuilt()) {
			OS::AssertFail();
		}
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		std::vector<RAL::Color4b> pixelsRGBA;
		pixelsRGBA.resize(width * height);
		std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));


		CreateComponent<ImGuiAtlasTexture>(entity0id, width, height, pixelsRGBA);

	};

	void CreateImGuiAtlasDriverTexture::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const ImGuiAtlasTexture* imGuiAtlasTexture0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
			.name_ = "",
			.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
			.data_ = std::vector<Common::Byte>{ (const Common::Byte*)imGuiAtlasTexture0->pixels_.data(), (const Common::Byte*)imGuiAtlasTexture0->pixels_.data() + imGuiAtlasTexture0->pixels_.size() * 4},
			.size_ = {
				imGuiAtlasTexture0->width_,
				imGuiAtlasTexture0->height_ },
			.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
			.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
			.targetStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader},
			.usages_ = { RAL::Driver::Texture::Usage::Sampled, RAL::Driver::Texture::Usage::TransferDestination },
			.mipLevels_ = 1
		};
		RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);


		CreateComponent<ImGuiAtlasDriverTexture>(entity0id, textureId);


	};

	void CreateImGuiAtlasTextureResource::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
		const ImGuiAtlasDriverTexture* imGuiAtlasDriverTexture0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {


		auto driver = renderDriver1->driver_;

		RAL::Driver::ResourceSet::Binding textureBinding
		{
			.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
			.binding_ = 0,
			.textureId_ = imGuiAtlasDriverTexture0->driverTextureId_
		};

		const RAL::Driver::Resource::Id textureResourceId = driver->CreateResource(textureBinding);

		CreateComponent<TextureResource>(entity0id, textureResourceId);

	};

	void UpdateImGuiRenderData::Update(
		ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
		ImGuiDriverVertexBuffer* imGuiDriverVertexBuffer,
		ImGuiDriverIndexBuffer* imGuiDriverIndexBuffer,
		ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

		static Common::UInt64 skipedFrames = 5;
		static Common::UInt64 currentFrame = 0;
		if (currentFrame != 4) {
			++currentFrame;
			return;
		}
		else {
			currentFrame = 0;
		}


		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr) {
			return;
		}
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		//TODO: Move to component.
		static Geom::IndexBuffer<Common::UInt32> indices;

		using namespace Common;
		indices.Reserve(10_MB / indices.GetIndexSize());
		indices.Clear();

		static Geom::VertexCloud<RAL::Vertex2ftc> vertices2ftc;

		vertices2ftc.Reserve(30_MB / vertices2ftc.GetVertexSize());
		vertices2ftc.Clear();

		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{

					const Common::Size verticesNumber = cmd_list->VtxBuffer.Size - pcmd->VtxOffset;

					indices.AddNextMesh(
						vertices2ftc.GetVerticesNumber(),
						cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
						cmd_list->IdxBuffer.Size - pcmd->IdxOffset);

					const ImDrawVert* imVertices = cmd_list->VtxBuffer.Data + pcmd->VtxOffset;
					vertices2ftc.Add((RAL::Vertex2ftc*)imVertices, verticesNumber);

				}
			}
		}
		if (vertices2ftc.GetVerticesNumber() == 0) {
			return;
		}

		auto driver = renderDriver->driver_;

		//Resize buffers if need.
		if (vertices2ftc.GetSizeInBytes() > renderDriver->driver_->GetVBSizeInBytes(imGuiDriverVertexBuffer->id_)) {
			renderDriver->driver_->ResizeVertexBuffer(
				imGuiDriverVertexBuffer->id_,
				vertices2ftc.GetSizeInBytes());
			imGuiDriverVertexBuffer->capacity_ = vertices2ftc.GetSizeInBytes();
		}
		if (indices.GetSizeInBytes() > renderDriver->driver_->GetIBSizeInBytes(imGuiDriverIndexBuffer->id_)) {
			renderDriver->driver_->ResizeIndexBuffer(
				imGuiDriverIndexBuffer->id_,
				indices.GetSizeInBytes());

			imGuiDriverIndexBuffer->capacity_ = vertices2ftc.GetSizeInBytes();
		}


		//Send vertices and indices data to GPU only if they changed.
		bool isDataChanged = false;

		//if()


		//Fill data.
		renderDriver->driver_->FillVertexBuffer(
			imGuiDriverVertexBuffer->id_,
			0,
			vertices2ftc.GetData(),
			vertices2ftc.GetVerticesNumber());
		imGuiDriverVertexBuffer->size_ = vertices2ftc.GetSizeInBytes();

		driver->FillIndexBuffer(
			imGuiDriverIndexBuffer->id_,
			0,
			indices.GetData(),
			indices.GetIndicesNumber());

		imGuiDriverIndexBuffer->size_ = indices.GetSizeInBytes();

	};

}