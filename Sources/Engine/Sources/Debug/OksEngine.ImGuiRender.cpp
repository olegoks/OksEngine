#pragma once
#include <Debug/auto_OksEngine.ImGuiRender.hpp>

#include <Debug/OksEngine.ImGuiTransform.hpp>

#include <Common.StringLiterals.hpp>
#include <Geometry.IndexBuffer.hpp>

#include <Render/Pipeline/OksEngine.Render.Pipeline.Utils.hpp>
#include <Render/Pipeline/OksEngine.Render.Shader.Utils.hpp>

namespace OksEngine
{
	namespace ImGUI {
		void CreatePipeline::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::Render::PipelineDescription::Manager::Tag* render__PipelineDescription__Manager__Tag0,
			const OksEngine::Render::PipelineDescription::Manager::Pipelines
			* render__PipelineDescription__Manager__Pipelines0,
			ECS2::Entity::Id entity1id,
			const OksEngine::ImGUI::State* state1,
			
			ECS2::Entity::Id entity2id,
			const OksEngine::RenderDriver* renderDriver2,
			
			ECS2::Entity::Id entity3id,
			const OksEngine::ResourceSystem* resourceSystem3) {

			auto driver = renderDriver2->driver_;

			auto pipelineIt = render__PipelineDescription__Manager__Pipelines0->nameToId_.find("ImGuiRenderPipeline");

			if (pipelineIt == render__PipelineDescription__Manager__Pipelines0->nameToId_.end()) {
				return;
			}
			const ECS2::Entity::Id pipelineDescriptionEntityId = pipelineIt->second;
			const ECS2::ComponentsFilter pipelineDescriptionCF = GetComponentsFilter(pipelineDescriptionEntityId);
			RAL::Driver::Pipeline::CI2 pipeline = RENDER__PIPELINEDESCRIPTION__CREATE_PIPELINE_CREATE_INFO2(pipelineDescriptionEntityId, 
				std::vector<RAL::Driver::Texture::Format>{ RAL::Driver::Texture::Format::RGBA_32_UNORM }, 
				RAL::Driver::Texture::Format::D_32_SFLOAT,
				RAL::Driver::Texture::Format::Undefined);
			const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipeline);

			CreateComponent<Pipeline>(entity1id, pipelineId);
		
		};


		void BeginRenderPass::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* state0,
			ECS2::Entity::Id entity1id, OksEngine::RenderDriver* renderDriver1,
			const OksEngine::Render::MultisamplingAttachment* render__MultisamplingAttachment1) {

			auto driver = renderDriver1->driver_;

			RAL::Driver::RenderPassInfo rpInfo;
			{
				rpInfo.renderOffset_ = { 0, 0 };
				rpInfo.renderArea_ = { 2560, 1440 };
				rpInfo.colorAttachments_ = {
					RAL::Driver::RenderPassAttachmentInfo{
						render__MultisamplingAttachment1->textureId_,
						RAL::Driver::Texture::State::DataForColorWrite,
						RAL::Driver::RP::AttachmentUsage::LoadOperation::Load,
						RAL::Driver::RP::AttachmentUsage::StoreOperation::Store
					}
				};
			}

			driver->BeginRenderPass2(rpInfo);

		}

		void RenderDrawData::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::ImGUI::State* state0,
			const OksEngine::ImGUI::Pipeline* pipeline0, 
			const OksEngine::Transform2DResource* transform2DResource0,
			const OksEngine::Render::Material::EntityId* render__Material__EntityId0,
			const OksEngine::ImGUI::DriverIndexBuffer* driverIndexBuffer0,
			const OksEngine::ImGUI::DriverVertexBuffer* driverVertexBuffer0,
			
			ECS2::Entity::Id entity1id,
			OksEngine::RenderDriver* renderDriver1,
			const OksEngine::Render::Material::ResourceSet_* render__Material__ResourceSet_1) {

			auto driver = renderDriver1->driver_;

			ImDrawData* draw_data = ImGui::GetDrawData();
			if (draw_data == nullptr) {
				return;
			}

			// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
			int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
			int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
			if (fb_width <= 0 || fb_height <= 0)
				return;

			//Skip rendering if main ImGui atlas is not created.
			const ECS2::ComponentsFilter materialComponentsFilter = GetComponentsFilter(render__Material__EntityId0->id_);
			if (!materialComponentsFilter.IsSet<Render::Material::InfoResourceSet>()) {
				return;
			}

			//TODO: Move to component.
			static Geom::IndexBuffer<Common::UInt32> indices;

			using namespace Common;
			indices.Reserve(10_MB / indices.GetIndexSize());
			indices.Clear();

			static Geom::VertexCloud<RAL::Vertex2ftc> vertices2ftc;

			vertices2ftc.Reserve(30_MB / vertices2ftc.GetVertexSize());
			vertices2ftc.Clear();


			driver->SetViewport(0, 0, 2560, 1440);
			driver->BindPipeline(pipeline0->id_);

			auto* materialInfoResourceSet = GetComponent<Render::Material::InfoResourceSet>(render__Material__EntityId0->id_);
			ImGuiIO& io = ImGui::GetIO();
			ImTextureData* texData = io.Fonts->TexData;
			texData->SetTexID(materialInfoResourceSet->info_.diffuseMapIndex_);

			Common::Size offsetVB = 0;
			Common::Size offsetIB = 0;


			for (int n = 0; n < draw_data->CmdListsCount; n++) {
				ImDrawList* cmd_list = draw_data->CmdLists[n];


				driver->FillVertexBuffer(
					driverVertexBuffer0->id_,
					offsetVB,
					cmd_list->VtxBuffer.Data,
					cmd_list->VtxBuffer.Size);

				driver->FillIndexBuffer(
					driverIndexBuffer0->id_,
					offsetIB,
					cmd_list->IdxBuffer.Data,
					cmd_list->IdxBuffer.Size);


				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
					{


						driver->BindVertexBuffer(driverVertexBuffer0->id_, offsetVB + pcmd->VtxOffset);
						driver->BindIndexBuffer(driverIndexBuffer0->id_, offsetIB + pcmd->IdxOffset);


						Common::UInt32 textureId = pcmd->GetTexID();


						driver->PushConstants(
							pipeline0->id_,
							RAL::Driver::Shader::Stage::FragmentShader,
							sizeof(Common::UInt32),
							&textureId);



						driver->Bind(pipeline0->id_, 0,
							{
								transform2DResource0->id_,				//set 0
								render__Material__ResourceSet_1->id_,	// set 1 // ALL TEXTURES
								materialInfoResourceSet->RSId_			// set 2 // 
							});


						driver->DrawIndexed(pcmd->ElemCount);

					}
				}

				offsetVB += cmd_list->VtxBuffer.Size;
				offsetIB += cmd_list->IdxBuffer.Size;
			}




			//const ECS2::ComponentsFilter textureComponentsFilter = GetComponentsFilter(GetComponent<Render::Texture::Type::DiffuseMap::EntityId>(render__Material__EntityId0->id_)->id_);



			driver->SetViewport(0, 0, 2560, 1440);
			driver->SetScissor(0, 0, 2560, 1440);	
			
			driver->BindVertexBuffer(driverVertexBuffer0->id_, 0);
			driver->BindIndexBuffer(driverIndexBuffer0->id_, 0);

			
			
			driver->Bind(pipeline0->id_, 0,
				{
					transform2DResource0->id_,				//set 0
					render__Material__ResourceSet_1->id_,	// set 1
					materialInfoResourceSet->RSId_			// set 2
				});
			driver->DrawIndexed(driverIndexBuffer0->size_ / sizeof(Common::UInt32));

		}


		void EndRenderPass::Update(ECS2::Entity::Id entity0id, const State* imGuiState0,
			ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

			auto driver = renderDriver1->driver_;
			
			driver->EndRenderPass2();


		}

		void CreateDriverVertexBuffer::Update(ECS2::Entity::Id entity1Id,
			const State* imGuiState,
			ECS2::Entity::Id entity2Id,
			RenderDriver* renderDriver) {


			auto driver = renderDriver->driver_;

			const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

			RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
				.verticesNumber_ = capacity,
				.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF3,
				.type_ = RAL::Driver::VertexBuffer::Type::Const
			};
			RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
			CreateComponent<DriverVertexBuffer>(entity1Id, VBId, capacity, 0);
		};


		void CreateDriverIndexBuffer::Update(ECS2::Entity::Id entity1Id,
			const State* imGuiState,
			ECS2::Entity::Id entity2Id,
			RenderDriver* renderDriver) {


			auto driver = renderDriver->driver_;

			using namespace Common;

			constexpr Common::UInt64 capacity = 128_MB;

			RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
				.indicesNumber_ = capacity,
				.indexType_ = RAL::Driver::IndexType::UI32,
				.type_ = RAL::Driver::IndexBuffer::Type::Const
			};
			const RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);

			CreateComponent<DriverIndexBuffer>(entity1Id, IBId, capacity, 0);

		};

		void UpdateDriverTransform2D::Update(
			ECS2::Entity::Id entity1Id, const State* imGuiState,
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


		void CreateDriverTransform2D::Update(
			ECS2::Entity::Id entity1Id,
			const State* imGuiState,
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

		namespace Atlas {

			void CreateTextureData::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0,

				ECS2::Entity::Id entity1id,
				OksEngine::ImGUI::State* state1) {

				ImGuiIO& io = ImGui::GetIO();
				io.Fonts->Build();
				ASSERT(io.Fonts->IsBuilt());
				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);



				std::vector<RAL::Color4b> pixelsRGBA;
				pixelsRGBA.resize(width * height);
				std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));

				const ECS2::Entity::Id materialEntityId = world_->CreateEntity<RENDER__MATERIAL__MATERIAL>();
				CreateComponent<Render::Material::EntityId>(entity1id, materialEntityId);
				
				CreateComponent<Render::Material::Tag>(materialEntityId);
				
				const ECS2::Entity::Id textureEntityId = world_->CreateEntity<RENDER__TEXTURE__TEXTURE>();
				CreateComponent<Render::Texture::Type::DiffuseMap::EntityId>(materialEntityId, textureEntityId);

				CreateComponent<Render::Texture::Tag>(textureEntityId);
				CreateComponent<Render::Texture::Type::DiffuseMap::Tag>(textureEntityId);
				CreateComponent<Render::Texture::Info>(textureEntityId, "ImGuiFontsAtlas");
				CreateComponent<Render::Texture::Data>(textureEntityId, width, height, pixelsRGBA);


				//RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
				//	.name_ = "",
				//	.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
				//	.data_ = std::vector<Common::Byte>{
				//		(const Common::Byte*)pixelsRGBA.data(),
				//		(const Common::Byte*)pixelsRGBA.data() + pixelsRGBA.size() * 4},
				//	.size_ = {
				//		width,
				//		height },
				//	.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
				//	.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
				//	.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader, RAL::Driver::Pipeline::Stage::VertexShader},
				//	.usages_ = { RAL::Driver::Texture::Usage::Sampled, RAL::Driver::Texture::Usage::TransferDestination },
				//	.mipLevels_ = 1,
				//	.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				//};
				//RAL::Driver::Texture::Id textureId = renderDriver0->driver_->CreateTexture(textureCreateInfo);

				//RAL::Driver::ResourceSet::Binding textureBinding
				//{
				//	.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				//	.binding_ = 0,
				//	.textureId_ = textureId
				//};

				//const RAL::Driver::Resource::Id textureResourceId = renderDriver0->driver_->CreateResource(textureBinding);

				//CreateComponent<Render::Material::ResourceSet>(entity1id, textureResourceId);


			};

			void CreateAtlasTextureResource::Update(
				ECS2::Entity::Id entity0id,
				const ImGUI::State* state0,
				const ImGUI::Atlas::TextureData* textureData0,

				ECS2::Entity::Id entity1id,
				RenderDriver* renderDriver1) {

				auto driver = renderDriver1->driver_;


				//RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
				//	.name_ = "",
				//	.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
				//	.data_ = std::vector<Common::Byte>{
				//		(const Common::Byte*)textureData0->pixels_.data(),
				//		(const Common::Byte*)textureData0->pixels_.data() + textureData0->pixels_.size() * 4},
				//	.size_ = {
				//		textureData0->width_,
				//		textureData0->height_ },
				//	.targetState_ = RAL::Driver::Texture::State::DataForShaderRead,
				//	.targetAccess_ = RAL::Driver::Texture::Access::ShaderRead,
				//	.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::FragmentShader},
				//	.usages_ = { RAL::Driver::Texture::Usage::Sampled, RAL::Driver::Texture::Usage::TransferDestination },
				//	.mipLevels_ = 1,
				//	.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				//};
				//RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);

				//RAL::Driver::ResourceSet::Binding textureBinding
				//{
				//	.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				//	.binding_ = 0,
				//	.textureId_ = textureId
				//};

				//const RAL::Driver::Resource::Id textureResourceId = driver->CreateResource(textureBinding);

				//CreateComponent<Render::Material::DiffuseMap::Resource>(entity0id, textureId, textureResourceId);
			}

		}


		void RemoveUIFromSelection::Update(
			ECS2::Entity::Id entity0id,
			const OksEngine::Render::Outline::State* render__Outline__State0,
			const OksEngine::Render::Outline::EnableSelection* render__Outline__EnableSelection0,
			const OksEngine::CursorEvents* cursorEvents0,
			const OksEngine::MouseEvents* mouseEvents0,

			ECS2::Entity::Id entity1id,
			OksEngine::RenderDriver* renderDriver1,
			const OksEngine::Render::Outline::IdsTextureRender::RenderPassId* render__Outline__IdsTextureRender__RenderPassId1,
			const OksEngine::Render::Outline::IdsTextureRender::UI::PipelineId* render__Outline__IdsTextureRender__PipelineId1,
			const OksEngine::Render::Outline::DataStorageBuffer* render__Outline__DataStorageBuffer1,
			const OksEngine::Render::Outline::DataStorageBufferResource* render__Outline__DataStorageBufferResource1,

			ECS2::Entity::Id entity2id,
			const OksEngine::MainWindow* mainWindow2,
			const OksEngine::MainWindowFramebufferSize* mainWindowFramebufferSize2,
			const OksEngine::MainWindowWorkAreaSize* mainWindowWorkAreaSize2,

			ECS2::Entity::Id entity3id,
			const OksEngine::ImGUI::State* state3, 
			const OksEngine::ImGUI::Pipeline* pipeline3,
			const OksEngine::MainMenuBar* mainMenuBar3,
			const OksEngine::Transform2DResource* transform2DResource3,
			const OksEngine::Render::Material::EntityId* render__Material__EntityId3,
			const OksEngine::ImGUI::DriverIndexBuffer* driverIndexBuffer3,
			const OksEngine::ImGUI::DriverVertexBuffer* driverVertexBuffer3) {

			auto driver = renderDriver1->driver_;

			Render::Outline::Data data{
				-1,
				-1,
				0,
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0 }
			};

			if (!cursorEvents0->events_.empty()) {


				float scaleX = 2560/*(float)mainWindowFramebufferSize1->width_*/ / (float)mainWindowWorkAreaSize2->width_;
				float scaleY = 1440/*(float)mainWindowFramebufferSize1->height_*/ / (float)mainWindowWorkAreaSize2->height_;

				// Вариант 1: Координаты в пикселях фреймбуфера (для прямого сравнения с gl_FragCoord)
				data.cursorPosX_ = cursorEvents0->events_.back().position_.x_ * scaleX;
				data.cursorPosY_ = (/*mainWindowWorkAreaSize1->height_ - */cursorEvents0->events_.back().position_.y_) * scaleY; // Инвертируем Y
				driver->StorageBufferWrite(
					render__Outline__DataStorageBuffer1->id_, 0, &data,
					//Write only cursor info.
					offsetof(Render::Outline::Data, Render::Outline::Data::selectedIds_));
			}

			driver->BindPipeline(render__Outline__IdsTextureRender__PipelineId1->id_);



			driver->Bind(render__Outline__IdsTextureRender__PipelineId1->id_, 0,
				{
					render__Outline__DataStorageBufferResource1->id_,
					transform2DResource3->id_
				}
			);


			driver->BindVertexBuffer(driverVertexBuffer3->id_, 0);
			driver->BindIndexBuffer(driverIndexBuffer3->id_, 0);


			driver->DrawIndexed(driverIndexBuffer3->size_ / sizeof(Common::UInt32));

		}

	}
}