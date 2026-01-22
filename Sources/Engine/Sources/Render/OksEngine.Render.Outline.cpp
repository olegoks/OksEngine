#pragma once
#include <Render\auto_OksEngine.Render.Outline.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Outline
		{
			void CreateIdsAttachment::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0) {

				RAL::Driver::Texture::CreateInfo1 renderBufferCreateInfo{
				.name_ = "IDs buffer",
				.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
				.size_ = { 2560, 1440 },
				.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
				.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
				.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
				.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment, RAL::Driver::Texture::Usage::TransferSource },
				.mipLevels_ = 1,
				.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				};
				const RAL::Driver::Texture::Id multisamplingBufferId = renderDriver0->driver_->CreateTexture(renderBufferCreateInfo);

				CreateComponent<IdsAttachment>(entity0id, multisamplingBufferId);

			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{
			void CreateAttachmentSet::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::IdsAttachment* idsAttachment0,
				const OksEngine::Render::RenderAttachment* render__RenderAttachment0, ECS2::Entity::Id entity1id,
				const OksEngine::RenderDriver* renderDriver1,
				const OksEngine::Render::Outline::RenderPassId* renderPassId1) {

				RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
					.rpId_ = renderPassId1->rpId_,
					.textures_ = { idsAttachment0->textureId_, render__RenderAttachment0->textureId_ },
					.size_ = glm::u32vec2{ 2560, 1440 }
				};

				RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = renderDriver0->driver_->CreateAttachmentSet(attachmentSetCI);

				CreateComponent<AttachmentSet>(entity1id, rpAttachmentsSetId);
			
			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{
			void CreateRenderPassId::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0) {
			

				auto driver = renderDriver0->driver_;

				std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
				{
					RAL::Driver::RP::AttachmentUsage idsAttachment{
						.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
						.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
						.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
						.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
						.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
						.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
					};
					attachmentsUsage.push_back(idsAttachment);

					RAL::Driver::RP::AttachmentUsage renderAttachment{
						.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
						.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
						.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Load,
						.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
						.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
						.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
					};
					attachmentsUsage.push_back(renderAttachment);

				}

				std::vector<RAL::Driver::RP::Subpass> subpasses;
				{
					RAL::Driver::RP::Subpass subpass0{
						.colorAttachments_ = { 0 } // Ids attachment.
					};
					subpasses.push_back(subpass0);
					RAL::Driver::RP::Subpass subpass1{
						.colorAttachments_ = { 1 } // Ids attachment.
					};
					subpasses.push_back(subpass1);
				}

				RAL::Driver::RP::CI rpCI{
					.name_ = "Outline render pass",
					.attachments_ = attachmentsUsage,
					.subpasses_ = subpasses
				};

				const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

				CreateComponent<RenderPassId>(entity0id, renderPassId);
			
			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{
			void CreatePipeline::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0, ECS2::Entity::Id entity1id,
				const OksEngine::ResourceSystem* resourceSystem1) {
			

				auto driver = renderDriver0->driver_;

				Resources::ResourceData imguiVertexShaderResource
					= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.vert");
				Resources::ResourceData imguiFragmentShaderResource
					= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.frag");

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

				RAL::Driver::Shader::Binding::Layout cameraBinding{
					.binding_ = 0,
					.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
					.stage_ = RAL::Driver::Shader::Stage::VertexShader
				};

				std::vector<RAL::Driver::PushConstant> pushConstants;
				{
					RAL::Driver::PushConstant pushConstant{
						.shaderStage_ = RAL::Driver::Shader::Stage::VertexShader,
						.offset_ = 0,
						.size_ = sizeof(ECS2::Entity::Id::ValueType)
					};
					pushConstants.emplace_back(pushConstant);
				}

				shaderBindings.push_back(cameraBinding);

				auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
				{
					multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1;
				}

				RAL::Driver::Pipeline::CI pipelineCI{
					.name_ = "Outline Pipeline",
					.renderPassId_ = renderPassId0->rpId_,
					.vertexShader_ = vertexShader,
					.fragmentShader_ = fragmentShader,
					.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
					.vertexType_ = RAL::Driver::VertexType::VF3,
					.indexType_ = RAL::Driver::IndexType::UI32,
					.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
					.cullMode_ = RAL::Driver::CullMode::Back,
					.shaderBindings_ = shaderBindings,
					.enableDepthTest_ = true,
					.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
					.multisamplingInfo_ = multisamplingInfo
				};

				const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

				CreateComponent<PipelineId>(entity0id, pipelineId);
			
			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{
			void BeginRenderPass::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0) {
			

				auto driver = renderDriver0->driver_;

				std::vector<RAL::Driver::RP::ClearValue> clearValues;
				{
					RAL::Driver::RP::ClearValue clearValue;
					{
						clearValue.color_.uint32[0] = 0;
						clearValue.color_.uint32[1] = 0;
						clearValue.color_.uint32[2] = 0;
						clearValue.color_.uint32[3] = 255;
					}
					clearValues.push_back(clearValue);
				}

				driver->BeginRenderPass(
					renderPassId0->rpId_,
					attachmentSet0->attachmentsSetId_,
					clearValues, { 0, 0 }, { 2560, 1440 });
			
			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{
			void RenderModelIds::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::EnableRegularRender* render__EnableRegularRender0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0, ECS2::Entity::Id entity1id,
				const OksEngine::Render::Debug::Debugger* debug__Debugger1,
				const OksEngine::Render::Debug::Enable* debug__Enable1,
				const OksEngine::Render::Debug::Lines::VertexBuffer* debug__Lines__VertexBuffer1,
				const OksEngine::Render::Debug::Lines::DriverVertexBuffer* debug__Lines__DriverVertexBuffer1,
				const OksEngine::Render::Debug::FlatShade::VertexBuffer* debug__FlatShade__VertexBuffer1,
				const OksEngine::Render::Debug::FlatShade::DriverVertexBuffer* debug__FlatShade__DriverVertexBuffer1,
				ECS2::Entity::Id entity2id, 
				const OksEngine::Camera* camera2, 
				const OksEngine::Active* active2,
				const OksEngine::DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer2,
				const OksEngine::CameraTransformResource* cameraTransformResource2) {
			

	//			auto driver = renderDriver0->driver_;

	//			driver->BindPipeline(pipelineId0->id_);

	//			driver->Bind(pipelineId0->id_, 0,
	//				{
	//					cameraTransformResource2->id_
	//				}
	//			);

	///*			driver->PushConstants(pipelineId0->id_, RAL::Driver::Shader::Stage::FragmentShader, 
	//				
	//				);*/

	//			driver->BindVertexBuffer(debug__FlatShade__DriverVertexBuffer1->id_, 0);


	//			driver->Draw(debug__FlatShade__VertexBuffer1->vertices_.GetVerticesNumber());

			
			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Outline
		{

			void BeginRenderOutlineSubpass::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0) {

				renderDriver0->driver_->NextSubpass();
			}

			void RenderOutline::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0) {

			}

			void EndRenderPass::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0) {
			

				auto driver = renderDriver0->driver_;

				driver->EndRenderPass();

			
			};

		}

	} // namespace Render

} // namespace OksEngine