#pragma once
#include <Render\auto_OksEngine.Render.Outline.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Outline
		{

			void CreateDataUniformBufferResource::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0) {


				RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
					.size_ = sizeof(Data),
					.type_ = RAL::Driver::UniformBuffer::Type::Mutable
				};
				RAL::Driver::UniformBuffer::Id UBId = renderDriver0->driver_->CreateUniformBuffer(UBCreateInfo);

				CreateComponent<DataUniformBuffer>(entity0id, UBId);



				RAL::Driver::ResourceSet::Binding dataUBBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.ubid_ = UBId,
					.offset_ = 0,
					.size_ = sizeof(Data)
				};

				RAL::Driver::Resource::Id resourceId = renderDriver0->driver_->CreateResource(dataUBBinding);

				CreateComponent<DataUniformBufferResource>(entity0id, resourceId);
			}

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

			void CreateDepthAttachment::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::RenderDriver* renderDriver0) {


				//DEPTH BUFFER
				RAL::Driver::Texture::CreateInfo1 depthBufferCreateInfo{
					.name_ = "Depth IDs buffer",
					.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
					.size_ = { 2560, 1440 },
					.targetState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
					.targetAccess_ = RAL::Driver::Texture::Access::DepthStencilWrite,
					.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::EarlyFragmentTests, RAL::Driver::Pipeline::Stage::LateFragmentTests },
					.usages_ = { RAL::Driver::Texture::Usage::DepthStencilAttachment },
					.mipLevels_ = 1,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				};

				const RAL::Driver::Texture::Id depthBufferId = renderDriver0->driver_->CreateTexture(depthBufferCreateInfo);

				CreateComponent<DepthAttachment>(entity0id, depthBufferId);

			}

			void CreateAttachmentSet::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::IdsAttachment* idsAttachment0,
				const OksEngine::Render::Outline::DepthAttachment* depthAttachment0,
				const OksEngine::Render::RenderAttachment* render__RenderAttachment0, ECS2::Entity::Id entity1id,
				const OksEngine::RenderDriver* renderDriver1,
				const OksEngine::Render::Outline::RenderPassId* renderPassId1) {

				RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
					.rpId_ = renderPassId1->rpId_,
					.textures_ = { idsAttachment0->textureId_, render__RenderAttachment0->textureId_, depthAttachment0->textureId_ },
					.size_ = glm::u32vec2{ 2560, 1440 }
				};

				RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = renderDriver0->driver_->CreateAttachmentSet(attachmentSetCI);

				CreateComponent<AttachmentSet>(entity1id, rpAttachmentsSetId);

			};

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

					RAL::Driver::RP::AttachmentUsage depthTestAttachment{
						.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
						.initialState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
						.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
						.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
						.finalState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
						.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
					};
					attachmentsUsage.push_back(depthTestAttachment);

				}

				std::vector<RAL::Driver::RP::Subpass> subpasses;
				{
					RAL::Driver::RP::Subpass subpass0{
						.colorAttachments_ = { 0 },// Ids attachment.
						.depthStencilAttachment_ = 2
					};
					subpasses.push_back(subpass0);
					RAL::Driver::RP::Subpass subpass1{
						.colorAttachments_ = { 1 } 
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

				RAL::Driver::Shader::Binding::Layout dataBinding{
					.binding_ = 0,
					.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
					.stage_ = RAL::Driver::Shader::Stage::VertexShader
				};

				std::vector<RAL::Driver::PushConstant> pushConstants;
				{
					RAL::Driver::PushConstant pushConstant{
						.shaderStage_ = RAL::Driver::Shader::Stage::VertexShader,
						.offset_ = 0,
						.size_ = sizeof(MeshInfo)
					};
					pushConstants.emplace_back(pushConstant);
				}

				shaderBindings.push_back(dataBinding);
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
					.pushConstants_ = pushConstants,
					.shaderBindings_ = shaderBindings,
					.enableDepthTest_ = true,
					.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
					.multisamplingInfo_ = multisamplingInfo
				};

				const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

				CreateComponent<PipelineId>(entity0id, pipelineId);

			};

			void BeginRenderPass::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0) {


				auto driver = renderDriver0->driver_;

				std::vector<RAL::Driver::RP::ClearValue> clearValues;
				{
					RAL::Driver::RP::ClearValue idsClearValue;
					{
						idsClearValue.color_.uint32[0] = 0;
						idsClearValue.color_.uint32[1] = 0;
						idsClearValue.color_.uint32[2] = 0;
						idsClearValue.color_.uint32[3] = 255;
					}
					clearValues.push_back(idsClearValue);

					RAL::Driver::RP::ClearValue clearValue;
					{
						clearValue.color_.uint32[0] = 0;
						clearValue.color_.uint32[1] = 0;
						clearValue.color_.uint32[2] = 0;
						clearValue.color_.uint32[3] = 255;
					}
					clearValues.push_back(clearValue);

					RAL::Driver::RP::ClearValue depthClearValue;
					{
						depthClearValue.depthStencil_.depth_ = 1.0f;
					}
					clearValues.push_back(depthClearValue);
				}

				driver->BeginRenderPass(
					renderPassId0->rpId_,
					attachmentSet0->attachmentsSetId_,
					clearValues, { 0, 0 }, { 2560, 1440 });

			};

			void RenderModelIds::Update(
				ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::EnableRegularRender* render__EnableRegularRender0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0,
				const OksEngine::Render::Outline::DataUniformBuffer* dataUniformBuffer0,
				const OksEngine::Render::Outline::DataUniformBufferResource* dataUniformBufferResource0,
				ECS2::Entity::Id entity1id, const OksEngine::Camera* camera1, const OksEngine::Active* active1,
				const OksEngine::DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
				const OksEngine::CameraTransformResource* cameraTransformResource1, ECS2::Entity::Id entity2id,
				const OksEngine::WorldPosition3D* worldPosition3D2, const OksEngine::WorldRotation3D* worldRotation3D2,
				const OksEngine::WorldScale3D* worldScale3D2,
				const OksEngine::Physics::DynamicRigidBody* physics__DynamicRigidBody2,
				const OksEngine::Physics::RigidBodyId* physics__RigidBodyId2,
				const OksEngine::Physics::PhysicsShape* physics__PhysicsShape2,
				const OksEngine::DriverVertexBuffer* driverVertexBuffer2,
				const OksEngine::DriverIndexBuffer* driverIndexBuffer2, const OksEngine::Vertices3D* vertices3D2,
				const OksEngine::Indices* indices2) {


				MeshInfo meshInfo{
					entity2id.GetRawValue(),
					worldPosition3D2->x_,
					worldPosition3D2->y_,
					worldPosition3D2->z_,
					worldRotation3D2->w_,
					worldRotation3D2->x_,
					worldRotation3D2->y_,
					worldRotation3D2->z_
				};

				auto driver = renderDriver0->driver_;

				driver->BindPipeline(pipelineId0->id_);

				driver->Bind(pipelineId0->id_, 0,
					{
						cameraTransformResource1->id_,
						dataUniformBufferResource0->id_
					}
				);

				driver->PushConstants(
					pipelineId0->id_,
					RAL::Driver::Shader::Stage::VertexShader,
					sizeof(meshInfo), &meshInfo);

				driver->BindVertexBuffer(driverVertexBuffer2->id_, 0);
				driver->BindIndexBuffer(driverIndexBuffer2->id_, 0);


				driver->DrawIndexed(indices2->indices_.GetIndicesNumber());


			};

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