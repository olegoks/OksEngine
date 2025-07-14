#pragma once
#include <Debug\Render\auto_OksEngine.ImGuiRender.hpp>

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


		RAL::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "ImGuiVertexShader",
			.type_ = RAL::Shader::Type::Vertex,
			.code_ = imguiVertexShader
		};
		auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

		RAL::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "ImGuiFragmentShader",
			.type_ = RAL::Shader::Type::Fragment,
			.code_ = imguiFragmentShader
		};
		auto fragmentShader = driver->CreateShader(fragmentShaderCreateInfo);

		std::vector<RAL::Driver::ShaderBinding::Layout> shaderBindings;

		RAL::Driver::ShaderBinding::Layout transformBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
			.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader
		};

		RAL::Driver::ShaderBinding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
			.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader
		};

		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::Pipeline::CI pipelineCI{
			.name_ = "ImGui Pipeline",
			.renderPassId_ = imGuiRenderPass0->rpId_,
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::TopologyType::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::None,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = true,
			.dbCompareOperation_ = RAL::Driver::DepthBuffer::CompareOperation::Always

		};

		const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

		CreateComponent<ImGuiPipeline>(entity0id, pipelineId);

	};

	void CreateImGuiRenderPass::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, ECS2::Entity::Id entity1id,
		const RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
		{
			RAL::Driver::RP::AttachmentUsage swapChainAttachment{
				.format_ = RAL::Driver::Format::RGBA_32,
				.initialState_ = RAL::Driver::RP::AttachmentUsage::State::DataIsUndefined,
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Ignore,
				.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
				.finalState_ = RAL::Driver::RP::AttachmentUsage::State::DataForCopyingSource
			};
			attachmentsUsage.push_back(swapChainAttachment);
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



		const auto [width, height] = driver->GetSwapChainTextureSize();

		std::vector<RAL::Color4b> pixels;
		pixels.resize(width * height, RAL::Color4b::Zero());

		RAL::Texture::CreateInfo textureCreateInfo{
			.name_ = "ImGui render buffer",
			.pixels_ = pixels,
			.size_ = { width, height }
		};
		const RAL::Texture::Id textureId = driver->CreateDiffuseMap(textureCreateInfo);

		RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
			.rpId_ = renderPassId,
			.textures_ = { textureId }
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

		auto swapChainTextureSize = driver->GetSwapChainTextureSize();

		driver->BeginRenderPass(imGuiRenderPass0->rpId_, imGuiRenderPass0->attachmentsSetId_, { 0, 0 }, swapChainTextureSize);
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

		//std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

		////CAMERA BINDING
		//{
		//	RAL::Driver::ShaderBinding::Data cameraBinding{
		//		.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
		//		.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
		//		.uniformBufferId_ = driverViewProjectionUniformBuffer->id_
		//	};
		//	shaderBindings.push_back(cameraBinding);
		//}

		//TRANSFORM BINDING
		{
			//struct Transform {
			//	glm::vec2 scale_;
			//	glm::vec2 translate_;
			//};

			//Transform transform{ 
			//	glm::vec2{ scale2D->x_, scale2D->y_ },
			//	glm::vec2{ position2D->x_, position2D->y_ }
			//};
			//RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			//	.size_ = sizeof(Transform),
			//	.type_ = RAL::Driver::UniformBuffer::Type::Mutable
			//};
			//RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);
			//driver->FillUniformBuffer(ubId, &transform);
			//RAL::Driver::ShaderBinding::Data transformBinding{
			//	.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
			//	.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
			//	.uniformBufferId_ = driverTransform2D->id_
			//};
			//shaderBindings.push_back(transformBinding);
		}

		////TEXTURE BINDING
		//{

		//	RAL::Driver::ShaderBinding::Data textureBinding{
		//		.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
		//		.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
		//		.textureId_ = driverTexture->driverTextureId_
		//	};

		//	shaderBindings.push_back(textureBinding);
		//}

		auto [width, height] = driver->GetSwapChainTextureSize();

		driver->SetViewport(0, 0, width, height);
		driver->SetScissor(0, 0, width, height);
		driver->BindPipeline(imGuiPipeline0->id_);
		driver->BindVertexBuffer(imGuiDriverVertexBuffer0->id_, 0);
		driver->BindIndexBuffer(imGuiDriverIndexBuffer0->id_, 0);
		driver->Bind(imGuiRenderPass0->rpId_,
			{
				transform2DResource0->id_,
				textureResource0->id_
			});
		driver->DrawIndexed(imGuiDriverIndexBuffer0->size_ / sizeof(Common::UInt32));

		//Common::Id driverMeshId = driver->DrawMesh(
		//	"ImGui Pipeline",
		//	driverVertexBuffer->id_,
		//	driverIndexBuffer->id_,
		//	shaderBindings
		//);
	}


	void EndImGuiRenderPass::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const ImGuiRenderPass* imGuiRenderPass0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		driver->EndSubpass();
		driver->EndRenderPass();

		driver->Show(imGuiRenderPass0->textureIds_[0]);

	}


}