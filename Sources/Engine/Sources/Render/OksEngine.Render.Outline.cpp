#pragma once
#include <Render\auto_OksEngine.Render.Outline.hpp>

#include <Debug\OksEngine.ImGuiTransform.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Outline
		{


			void EditSelectedEntityIds(
				std::shared_ptr<ECS2::World> ecsWorld,
				SelectedEntityIds* selectedEntityIds) {

				ImGui::PushID(SelectedEntityIds::GetTypeId());
				ImGui::Indent(20);
				for (ECS2::Entity::Id entityId : selectedEntityIds->ids_) {
					EditEntity(ecsWorld, entityId);
				}
				ImGui::Unindent(20);
				ImGui::PopID();

			}

			void CreateState::Update() {

				const ECS2::Entity::Id stateEntityId = CreateEntity();

				CreateComponent<State>(stateEntityId);
				CreateComponent<EnableSelection>(stateEntityId);
				CreateComponent<SelectedEntityIds>(stateEntityId);
				CreateComponent<HandleCursorEvents>(stateEntityId);
				CreateComponent<HandleMouseEvents>(stateEntityId);
				CreateComponent<Input::HandleKeyboardEvents>(stateEntityId);

			}

			void GetSelectedEntityIds::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Render::Outline::State* state0,
				const OksEngine::Render::Outline::EnableSelection* enableSelection0,
				const OksEngine::CursorEvents* cursorEvents0,
				OksEngine::MouseEvents* mouseEvents0,
				const OksEngine::Input::KeyboardEvents* input__KeyboardEvents0,
				OksEngine::Render::Outline::SelectedEntityIds* selectedEntityIds0,

				ECS2::Entity::Id entity1id,
				const OksEngine::RenderDriver* renderDriver1,
				const OksEngine::Render::Outline::DataStorageBuffer* dataStorageBuffer1) {


				Data data;

				renderDriver1->driver_->StorageBufferRead(
					dataStorageBuffer1->id_,
					0, sizeof(Data),
					&data);

				bool isLeftButtonClicked = false;
				{
					while (!mouseEvents0->events_.empty()) {
						const auto& event = mouseEvents0->events_.back();
						if (event.key_ == UIAL::Window::MouseKey::Left &&
							(event.event_ == UIAL::Window::KeyAction::Pressed || event.event_ == UIAL::Window::KeyAction::Hold)) {
							isLeftButtonClicked = true;
							break;
						}
						mouseEvents0->events_.pop();
					}
				}

				bool isShiftPressed = false;
				for (const auto& keyboardEvent : input__KeyboardEvents0->events_) {
					if (keyboardEvent.first == UIAL::Window::KeyboardKey::LEFT_SHIFT &&
						(keyboardEvent.second == UIAL::Window::KeyAction::Pressed || keyboardEvent.second == UIAL::Window::KeyAction::Hold)) {
						isShiftPressed = true;
						break;
					}
				}

				if (isLeftButtonClicked && !isShiftPressed) {
					if (data.potencialSelectedId_ != 0) {
						selectedEntityIds0->ids_ = { data.potencialSelectedId_ };
					}
					else {
						//selectedEntityIds0->ids_.clear();
					}
				}
				else if (!selectedEntityIds0->ids_.empty() && isLeftButtonClicked && isShiftPressed) {
					bool containsYet = false;
					{
						for (const auto& id : selectedEntityIds0->ids_) {
							if (ECS2::Entity::Id{ data.potencialSelectedId_ } == id) {
								containsYet = true;
								break;
							}
						}
					}
					if (data.potencialSelectedId_ != 0) {
						if (!containsYet) {
							selectedEntityIds0->ids_.push_back(ECS2::Entity::Id{ data.potencialSelectedId_ });
						}
					}
					else {
						//selectedEntityIds0->ids_.clear();
					}
					
				}



				if (selectedEntityIds0->ids_.size() > data.selectedIds_.max_size()) {
					selectedEntityIds0->ids_.resize(data.selectedIds_.max_size());
					OS::LogWarning("render/outline", "Reached max limit of selected entities.");
				}
				data.selectedIds_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0 };
				std::memcpy(
					data.selectedIds_.data(),
					selectedEntityIds0->ids_.data(),
					selectedEntityIds0->ids_.size() * sizeof(ECS2::Entity::Id::ValueType));

				renderDriver1->driver_->StorageBufferWrite(
					dataStorageBuffer1->id_, 0,
					&data, sizeof(Data));

			}

			void CreateDataStorageBufferResource::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0) {


				RAL::Driver::SB::CI SBCreateInfo{
					.size_ = sizeof(Data)
				};
				RAL::Driver::SB::Id SBId = renderDriver0->driver_->CreateStorageBuffer(SBCreateInfo);

				CreateComponent<DataStorageBuffer>(entity0id, SBId);



				RAL::Driver::ResourceSet::Binding dataSBBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
					.binding_ = 0,
					.sbid_ = SBId,
					.offset_ = 0,
					.size_ = sizeof(Data)
				};

				RAL::Driver::Resource::Id resourceId = renderDriver0->driver_->CreateResource(dataSBBinding);

				CreateComponent<DataStorageBufferResource>(entity0id, resourceId);
			}

			namespace IdsTextureRender {

				namespace UI {
					void CreatePipeline::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
						const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId0,
						ECS2::Entity::Id entity1id, const OksEngine::ResourceSystem* resourceSystem1) {


						auto driver = renderDriver0->driver_;

						Resources::ResourceData imguiVertexShaderResource
							= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.IdsAttachment.UI.vert");
						Resources::ResourceData imguiFragmentShaderResource
							= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.IdsAttachment.UI.frag");

						std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
						std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };


						RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
							.name_ = "ImGuiOutlineVertexShader",
							.type_ = RAL::Driver::Shader::Type::Vertex,
							.code_ = imguiVertexShader
						};
						auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

						RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
							.name_ = "ImGuiOutlineFragmentShader",
							.type_ = RAL::Driver::Shader::Type::Fragment,
							.code_ = imguiFragmentShader
						};
						auto fragmentShader = driver->CreateShader(fragmentShaderCreateInfo);

						std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

						RAL::Driver::Shader::Binding::Layout dataBinding{
							.binding_ = 0,
							.type_ = RAL::Driver::Shader::Binding::Type::Storage,
							.stage_ = RAL::Driver::Shader::Stage::FragmentShader
						};

						RAL::Driver::Shader::Binding::Layout transformBinding{
							.binding_ = 0,
							.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
							.stage_ = RAL::Driver::Shader::Stage::VertexShader
						};

						std::vector<RAL::Driver::PushConstant> pushConstants;
						/*{
							RAL::Driver::PushConstant transform{
								.shaderStage_ = RAL::Driver::Shader::Stage::VertexShader,
								.offset_ = 0,
								.size_ = sizeof(ImGuiTransform)
							};
							pushConstants.push_back(transform);
						}*/



						shaderBindings.push_back(dataBinding);
						shaderBindings.push_back(transformBinding);

						auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
						{
							multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1;
						}

						RAL::Driver::Pipeline::CI pipelineCI{
							.name_ = "ImGui Outline Pipeline",
							.renderPassId_ = renderPassId0->rpId_,
							.vertexShader_ = vertexShader,
							.fragmentShader_ = fragmentShader,
							.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
							//TODO: change vertex type to VF2
							.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF3,
							.indexType_ = RAL::Driver::IndexType::UI32,
							.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
							.cullMode_ = RAL::Driver::CullMode::None,
							.pushConstants_ = pushConstants,
							.shaderBindings_ = shaderBindings,
							.enableDepthTest_ = true,
							.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always,
							//.multisamplingInfo_ = multisamplingInfo
						};

						const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

						CreateComponent<PipelineId>(entity0id, pipelineId);

					}
				}

				void CreateIdsAttachment::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0) {

					RAL::Driver::Texture::CreateInfo1 renderBufferCreateInfo{
					.name_ = "IDs buffer",
					.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
					.size_ = { 2560, 1440 },
					.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
					.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
					.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
					.usages_ = {
							RAL::Driver::Texture::Usage::Sampled,
							RAL::Driver::Texture::Usage::ColorAttachment },
					.mipLevels_ = 1,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
					};
					const RAL::Driver::Texture::Id idsTextureId = renderDriver0->driver_->CreateTexture(renderBufferCreateInfo);

					CreateComponent<IdsAttachment>(entity0id, idsTextureId);

					RAL::Driver::Resource::Binding idsTextureBinding
					{
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
						.binding_ = 0,
						.textureId_ = idsTextureId
					};

					const RAL::Driver::Resource::Id textureRSId = renderDriver0->driver_->CreateResource(idsTextureBinding);

					CreateComponent<IdsAttachmentResource>(entity0id, textureRSId);
				};

				void CreateDepthAttachment::Update(
					ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0) {


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
					const OksEngine::Render::Outline::IdsTextureRender::IdsAttachment* idsAttachment0,
					const OksEngine::Render::Outline::IdsTextureRender::DepthAttachment* depthAttachment0,
					const OksEngine::Render::RenderAttachment* render__RenderAttachment0, ECS2::Entity::Id entity1id,
					const OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId1) {

					RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
						.rpId_ = renderPassId1->rpId_,
						.textures_ = {
							idsAttachment0->textureId_,
							depthAttachment0->textureId_
						},
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

						//RAL::Driver::RP::AttachmentUsage renderAttachment{
						//	.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
						//	.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
						//	.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Load,
						//	.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
						//	.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
						//	.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
						//};
						//attachmentsUsage.push_back(renderAttachment);

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
							.colorAttachments_ = { { 0, RAL::Driver::Texture_State::DataForColorWrite }  },// Ids attachment.
							.depthStencilAttachment_ = { 1, RAL::Driver::Texture_State::DataForDepthStencilWrite }
						};
						subpasses.push_back(subpass0);
					}

					std::vector<RAL::Driver::RP::Subpass::Dependency> dependencies;
					{
						{
							RAL::Driver::RP::Subpass::Dependency fromExternalToFirst{
								.fromSubpassIndex_ = RAL::Driver::RP::Subpass::external_,
								.fromPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput,
								.fromAccess_ = RAL::Driver::Texture::Access::ColorWrite,

								.toSubpassIndex_ = 0,
								.toPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput,
								.toAccess_ = RAL::Driver::Texture::Access::ColorWrite,
							};
							dependencies.push_back(fromExternalToFirst);
						}
						{
							RAL::Driver::RP::Subpass::Dependency fromFirstToExternal{
								.fromSubpassIndex_ = 0,
								.fromPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput,
								.fromAccess_ = RAL::Driver::Texture::Access::ColorWrite,

								.toSubpassIndex_ = RAL::Driver::RP::Subpass::external_,
								.toPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, // First potencial use of attachments.
								.toAccess_ = RAL::Driver::Texture::Access::ColorWrite,
							};
							dependencies.push_back(fromFirstToExternal);
						}
					}


					RAL::Driver::RP::CI rpCI{
						.name_ = "Outline render pass",
						.attachments_ = attachmentsUsage,
						.subpasses_ = subpasses,
						.subpassDependecies_ = dependencies
					};

					const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

					CreateComponent<RenderPassId>(entity0id, renderPassId);

				};

				void CreatePipeline::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
					const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId0,
					ECS2::Entity::Id entity1id, const OksEngine::ResourceSystem* resourceSystem1) {


					auto driver = renderDriver0->driver_;

					Resources::ResourceData imguiVertexShaderResource
						= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.IdsAttachment.vert");
					Resources::ResourceData imguiFragmentShaderResource
						= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.IdsAttachment.frag");

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
						.type_ = RAL::Driver::Shader::Binding::Type::Storage,
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader
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

					shaderBindings.push_back(cameraBinding);
					shaderBindings.push_back(dataBinding);

					auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
					{
						multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1;
					}

					RAL::Driver::Pipeline::CI pipelineCI{
						.name_ = "Ids texture render Pipeline",
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

				void BeginRenderPass::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Outline::State* state0,
					const OksEngine::Render::Outline::EnableSelection* enableSelection0, ECS2::Entity::Id entity1id,
					OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId1,
					const OksEngine::Render::Outline::IdsTextureRender::AttachmentSet* attachmentSet1,
					const OksEngine::Render::Outline::IdsTextureRender::IdsAttachment* idsAttachment1,
					const OksEngine::Render::Outline::IdsTextureRender::PipelineId* pipelineId1) {

					auto driver = renderDriver1->driver_;

					//renderDriver0->driver_->TextureMemoryBarrier(
					//	idsAttachment0->textureId_,
					//	RAL::Driver::Texture::State::DataForShaderRead, RAL::Driver::Texture::State::DataForColorWrite,
					//	RAL::Driver::Texture::Access::ShaderRead, RAL::Driver::Texture::Access::ColorWrite,
					//	RAL::Driver::Pipeline::Stage::FragmentShader, RAL::Driver::Pipeline::Stage::ColorAttachmentOutput);

					//renderDriver0->driver_->TextureMemoryBarrier(
					//	idsAttachment0->textureId_,
					//	RAL::Driver::Texture::State::DataForColorWrite, RAL::Driver::Texture::State::DataForShaderRead,
					//	RAL::Driver::Texture::Access::ColorWrite, RAL::Driver::Texture::Access::ShaderRead,
					//	RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, RAL::Driver::Pipeline::Stage::FragmentShader);

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

						//RAL::Driver::RP::ClearValue clearValue;
						//{
						//	clearValue.color_.uint32[0] = 0;
						//	clearValue.color_.uint32[1] = 0;
						//	clearValue.color_.uint32[2] = 0;
						//	clearValue.color_.uint32[3] = 255;
						//}
						//clearValues.push_back(clearValue);

						RAL::Driver::RP::ClearValue depthClearValue;
						{
							depthClearValue.depthStencil_.depth_ = 1.0f;
						}
						clearValues.push_back(depthClearValue);
					}

					driver->BeginDebugLabel(
						RAL::Color3f{ 1.0, 0.0, 0.0 },
						"Begin outline texture render pass.");

					driver->BeginRenderPass(
						renderPassId1->rpId_,
						attachmentSet1->attachmentsSetId_,
						clearValues, { 0, 0 }, { 2560, 1440 });

				};


				void EndRenderPass::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Outline::State* state0,
					const OksEngine::Render::Outline::EnableSelection* enableSelection0, ECS2::Entity::Id entity1id,
					OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* renderPassId1,
					const OksEngine::Render::Outline::IdsTextureRender::PipelineId* pipelineId1,
					const OksEngine::Render::Outline::IdsTextureRender::IdsAttachment* idsAttachment1) {

					auto driver = renderDriver1->driver_;

					driver->EndRenderPass();

					driver->EndDebugLabel();

					renderDriver1->driver_->TextureMemoryBarrier(
						idsAttachment1->textureId_,
						RAL::Driver::Texture::State::DataForColorWrite, RAL::Driver::Texture::State::DataForShaderRead,
						RAL::Driver::Texture::Access::ColorWrite, RAL::Driver::Texture::Access::ShaderRead,
						RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, RAL::Driver::Pipeline::Stage::FragmentShader);
					//renderDriver0->driver_->TextureMemoryBarrier(
					//	idsAttachment0->textureId_,
					//	RAL::Driver::Texture::State::DataForColorWrite, RAL::Driver::Texture::State::DataForShaderRead,
					//	RAL::Driver::Texture::Access::ColorWrite, RAL::Driver::Texture::Access::ShaderRead,
					//	RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, RAL::Driver::Pipeline::Stage::FragmentShader);

					/*renderDriver0->driver_->TextureMemoryBarrier(
						idsAttachment0->textureId_,
						RAL::Driver::Texture::State::DataForColorWrite, RAL::Driver::Texture::State::DataForColorWrite,
						RAL::Driver::Texture::Access::ColorWrite, RAL::Driver::Texture::Access::ColorWrite,
						RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, RAL::Driver::Pipeline::Stage::ColorAttachmentOutput);*/

				};

			}


			namespace OutlineRender {

				void CreateAttachmentSet::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
					const OksEngine::Render::MultisamplingAttachment* render__MultisamplingAttachment0,
					ECS2::Entity::Id entity1id, const OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::OutlineRender::RenderPassId* renderPassId1) {

					RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
						.rpId_ = renderPassId1->rpId_,
						.textures_ = {
							render__MultisamplingAttachment0->textureId_
						},
						.size_ = glm::u32vec2{ 2560, 1440 }
					};

					RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = renderDriver0->driver_->CreateAttachmentSet(attachmentSetCI);

					CreateComponent<AttachmentSet>(entity1id, rpAttachmentsSetId);

				};

				void CreateRenderPassId::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0) {


					auto driver = renderDriver0->driver_;

					std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
					{
						RAL::Driver::RP::AttachmentUsage renderAttachment{
							.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
							.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
							.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Load,
							.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
							.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
							.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
						};
						attachmentsUsage.push_back(renderAttachment);
					}

					std::vector<RAL::Driver::RP::Subpass> subpasses;
					{
						RAL::Driver::RP::Subpass subpass0{
							.colorAttachments_ = { { 0, RAL::Driver::Texture_State::DataForColorWrite }  }
						};
						subpasses.push_back(subpass0);
					}

					std::vector<RAL::Driver::RP::Subpass::Dependency> dependencies;
					{
						{
							RAL::Driver::RP::Subpass::Dependency fromExternalToFirst{
								.fromSubpassIndex_ = RAL::Driver::RP::Subpass::external_,
								.fromPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput,
								.fromAccess_ = RAL::Driver::Texture::Access::ColorWrite,

								.toSubpassIndex_ = 0,
								.toPipelineStage_ = RAL::Driver::Pipeline::Stage::VertexShader,
								.toAccess_ = RAL::Driver::Texture::Access::ShaderRead,
							};
							dependencies.push_back(fromExternalToFirst);
						}
						{
							RAL::Driver::RP::Subpass::Dependency fromFirstToExternal{
								.fromSubpassIndex_ = 0,
								.fromPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput,
								.fromAccess_ = RAL::Driver::Texture::Access::ColorWrite,

								.toSubpassIndex_ = RAL::Driver::RP::Subpass::external_,
								.toPipelineStage_ = RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, // First potencial use of attachments.
								.toAccess_ = RAL::Driver::Texture::Access::ColorWrite,
							};
							dependencies.push_back(fromFirstToExternal);
						}
					}


					RAL::Driver::RP::CI rpCI{
						.name_ = "Outline render pass",
						.attachments_ = attachmentsUsage,
						.subpasses_ = subpasses,
						.subpassDependecies_ = dependencies
					};

					const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

					CreateComponent<RenderPassId>(entity0id, renderPassId);

				};

				void CreatePipeline::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
					const OksEngine::Render::Outline::OutlineRender::RenderPassId* renderPassId0,
					ECS2::Entity::Id entity1id, const OksEngine::ResourceSystem* resourceSystem1) {


					auto driver = renderDriver0->driver_;

					Resources::ResourceData imguiVertexShaderResource
						= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.vert");
					Resources::ResourceData imguiFragmentShaderResource
						= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Render.Outline.frag");

					std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
					std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };


					RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
						.name_ = "RenderOutlineVertexShader",
						.type_ = RAL::Driver::Shader::Type::Vertex,
						.code_ = imguiVertexShader
					};
					auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

					RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
						.name_ = "RenderOutlineFragmentShader",
						.type_ = RAL::Driver::Shader::Type::Fragment,
						.code_ = imguiFragmentShader
					};
					auto fragmentShader = driver->CreateShader(fragmentShaderCreateInfo);

					std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

					RAL::Driver::Shader::Binding::Layout idsTexture{
						.binding_ = 0,
						.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader
					};

					RAL::Driver::Shader::Binding::Layout dataBinding{
						.binding_ = 0,
						.type_ = RAL::Driver::Shader::Binding::Type::Storage,
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader
					};

					std::vector<RAL::Driver::PushConstant> pushConstants;
					{
						RAL::Driver::PushConstant pushConstant{
							.shaderStage_ = RAL::Driver::Shader::Stage::FragmentShader,
							.offset_ = 0,
							.size_ = sizeof(OutlineSettings)
						};
						pushConstants.emplace_back(pushConstant);
					}

					shaderBindings.push_back(idsTexture);
					shaderBindings.push_back(dataBinding);

					auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
					{
						multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1;
					}

					RAL::Driver::Pipeline::CI pipelineCI{
						.name_ = "Outline render Pipeline",
						.renderPassId_ = renderPassId0->rpId_,
						.subpassIndex_ = 0,
						.vertexShader_ = vertexShader,
						.fragmentShader_ = fragmentShader,
						.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
						.vertexType_ = RAL::Driver::VertexType::Undefined,
						.indexType_ = RAL::Driver::IndexType::Undefined,
						.frontFace_ = RAL::Driver::FrontFace::Clockwise,
						.cullMode_ = RAL::Driver::CullMode::None,
						.pushConstants_ = pushConstants,
						.shaderBindings_ = shaderBindings,
						.enableDepthTest_ = false,
						.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always,
						.multisamplingInfo_ = multisamplingInfo
					};

					const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

					CreateComponent<PipelineId>(entity0id, pipelineId);

				}

				void BeginRenderPass::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Outline::State* state0,
					const OksEngine::Render::Outline::EnableSelection* enableSelection0, ECS2::Entity::Id entity1id,
					OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::OutlineRender::RenderPassId* renderPassId1,
					const OksEngine::Render::Outline::OutlineRender::AttachmentSet* attachmentSet1,
					const OksEngine::Render::Outline::OutlineRender::PipelineId* pipelineId1) {


					auto driver = renderDriver1->driver_;

					std::vector<RAL::Driver::RP::ClearValue> clearValues;

					/*		return;*/
					driver->BeginRenderPass(
						renderPassId1->rpId_,
						attachmentSet1->attachmentsSetId_,
						clearValues, { 0, 0 }, { 2560, 1440 });

				}


				void RenderOutline::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Outline::State* state0,
					const OksEngine::Render::Outline::EnableSelection* enableSelection0, ECS2::Entity::Id entity1id,
					OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::DataStorageBuffer* dataStorageBuffer1,
					const OksEngine::Render::Outline::DataStorageBufferResource* dataStorageBufferResource1,
					const OksEngine::Render::Outline::OutlineRender::RenderPassId* renderPassId1,
					const OksEngine::Render::Outline::IdsTextureRender::IdsAttachmentResource
					* idsTextureRender__IdsAttachmentResource1,
					const OksEngine::Render::Outline::OutlineRender::AttachmentSet* attachmentSet1,
					const OksEngine::Render::Outline::OutlineRender::PipelineId* pipelineId1) {

					auto driver = renderDriver1->driver_;

					driver->BindPipeline(pipelineId1->id_);


					driver->Bind(pipelineId1->id_, 0,
						{
							idsTextureRender__IdsAttachmentResource1->id_,
							dataStorageBufferResource1->id_
						}
					);

					OutlineSettings outlineSettings{
						1.0
					};

					driver->PushConstants(
						pipelineId1->id_,
						RAL::Driver::Shader::Stage::FragmentShader,
						sizeof(OutlineSettings), &outlineSettings);

					driver->Draw(3);
				}

				void EndRenderPass::Update(ECS2::Entity::Id entity0id, const OksEngine::Render::Outline::State* state0,
					const OksEngine::Render::Outline::EnableSelection* enableSelection0, ECS2::Entity::Id entity1id,
					OksEngine::RenderDriver* renderDriver1,
					const OksEngine::Render::Outline::OutlineRender::RenderPassId* renderPassId1,
					const OksEngine::Render::Outline::OutlineRender::PipelineId* pipelineId1,
					const OksEngine::Render::Outline::IdsTextureRender::IdsAttachment* idsTextureRender__IdsAttachment1) {


					auto driver = renderDriver1->driver_;
					driver->EndRenderPass();

					//Return ids buffer state to RAL::Driver::Texture::State::DataForColorWrite for the next frame.
					renderDriver1->driver_->TextureMemoryBarrier(
						idsTextureRender__IdsAttachment1->textureId_,
						RAL::Driver::Texture::State::DataForShaderRead, RAL::Driver::Texture::State::DataForColorWrite,
						RAL::Driver::Texture::Access::ShaderRead, RAL::Driver::Texture::Access::ColorWrite,
						RAL::Driver::Pipeline::Stage::FragmentShader, RAL::Driver::Pipeline::Stage::ColorAttachmentOutput);


				};

			}
			/*void BeginRenderOutlineSubpass::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::Outline::RenderPassId* renderPassId0,
				const OksEngine::Render::Outline::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Outline::PipelineId* pipelineId0,
				const OksEngine::Render::Outline::IdsAttachment* idsAttachment0) {

				renderDriver0->driver_->TextureMemoryBarrier(
					idsAttachment0->textureId_,
					RAL::Driver::Texture::State::DataForColorWrite, RAL::Driver::Texture::State::DataForShaderRead,
					RAL::Driver::Texture::Access::ColorWrite, RAL::Driver::Texture::Access::ShaderRead,
					RAL::Driver::Pipeline::Stage::ColorAttachmentOutput, RAL::Driver::Pipeline::Stage::FragmentShader);

				renderDriver0->driver_->NextSubpass();
			}*/







		}

	} // namespace Render

} // namespace OksEngine