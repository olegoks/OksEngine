#pragma once
#include <Render\auto_OksEngine.Render.Debug.hpp>

namespace OksEngine
{


	namespace Render
	{
		namespace Debug
		{
			void CreateRenderer::Update() {

				const ECS2::Entity::Id rendererId = CreateEntity();
				CreateComponent<Renderer>(rendererId);
				CreateComponent<VertexBuffer>(rendererId);

			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Debug
		{
			void CreateDriverVertexBuffer::Update(ECS2::Entity::Id entity0id, const Renderer* renderer0, ECS2::Entity::Id entity1id,
				RenderDriver* renderDriver1) {

				auto driver = renderDriver1->driver_;

				const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

				RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
					.verticesNumber_ = capacity,
					.vertexType_ = RAL::Driver::VertexType::VF3_NF3_CF4,
					.type_ = RAL::Driver::VertexBuffer::Type::Const
				};
				RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
				CreateComponent<DriverVertexBuffer>(entity0id, VBId, capacity, 0);


			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Debug
		{
			void UpdateDriverVertexBuffer::Update(
				ECS2::Entity::Id entity0id, const Render::Debug::Renderer* renderer0,
				Render::Debug::VertexBuffer* vertexBuffer0, Render::Debug::DriverVertexBuffer* driverVertexBuffer0,
				ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {


				if (vertexBuffer0->vertices_.GetVerticesNumber() == 0) {
					return;
				}

				auto driver = renderDriver1->driver_;

				//Resize buffers if need.
				if (vertexBuffer0->vertices_.GetSizeInBytes() > driver->GetVBSizeInBytes(driverVertexBuffer0->id_)) {
					driver->ResizeVertexBuffer(
						driverVertexBuffer0->id_,
						vertexBuffer0->vertices_.GetSizeInBytes());
					driverVertexBuffer0->capacity_ = vertexBuffer0->vertices_.GetSizeInBytes();
				}

				//Send vertices and indices data to GPU only if they changed.
				bool isDataChanged = false;

				//Fill data.
				driver->FillVertexBuffer(
					driverVertexBuffer0->id_,
					0,
					vertexBuffer0->vertices_.GetData(),
					vertexBuffer0->vertices_.GetVerticesNumber());
				driverVertexBuffer0->size_ = vertexBuffer0->vertices_.GetSizeInBytes();


			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Debug
		{
			void CreatePipeline::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0,
				const Render::MainRenderPass* render__MainRenderPass0, ECS2::Entity::Id entity1id,
				const ResourceSystem* resourceSystem1, ECS2::Entity::Id entity2id,
				const Render::Debug::Renderer* renderer2) {



				Resources::ResourceData vertexShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/flatShaded.vert");
				Resources::ResourceData fragmentShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/flatShaded.frag");

				auto driver = renderDriver0->driver_;

				std::string imguiVertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
				std::string imguiFragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };

				RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
					.name_ = "FlatShadedVertexShader",
					.type_ = RAL::Driver::Shader::Type::Vertex,
					.code_ = imguiVertexShader
				};
				auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

				RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
					.name_ = "FlatShadedFragmentShader",
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
				shaderBindings.push_back(cameraBinding);
				auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
				{
					multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8;
				}

				RAL::Driver::Pipeline::CI pipelineCI{
					.name_ = "Flat shaded Pipeline",
					.renderPassId_ = render__MainRenderPass0->rpId_,
					.vertexShader_ = vertexShader,
					.fragmentShader_ = fragmentShader,
					.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
					.vertexType_ = RAL::Driver::VertexType::VF3_NF3_CF4,
					.indexType_ = RAL::Driver::IndexType::UI32,
					.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
					.cullMode_ = RAL::Driver::CullMode::Back,
					.shaderBindings_ = shaderBindings,
					.enableDepthTest_ = true,
					.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
					.multisamplingInfo_ = multisamplingInfo
				};

				const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

				CreateComponent<Pipeline>(entity2id, pipelineId);



			};

		}

	} // namespace Render

	namespace Render
	{
		namespace Debug
		{
			void ClearVertexBuffer::Update(ECS2::Entity::Id entity0id, const Render::Debug::Renderer* renderer0,
				Render::Debug::VertexBuffer* vertexBuffer0) {

				vertexBuffer0->vertices_.Clear();

			}
		}
	}



	namespace Render
	{
		namespace Debug
		{
			void Draw::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0,
				const Render::MainRenderPass* render__MainRenderPass0, ECS2::Entity::Id entity1id,
				const Camera* camera1, const Active* active1,
				const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
				const CameraTransformResource* cameraTransformResource1, ECS2::Entity::Id entity2id,
				const Render::Debug::Renderer* renderer2, const Render::Debug::Enable* enable2,
				const Render::Debug::Pipeline* pipeline2, const Render::Debug::VertexBuffer* vertexBuffer2,
				const Render::Debug::DriverVertexBuffer* driverVertexBuffer2) {


				auto driver = renderDriver0->driver_;

				driver->BindPipeline(pipeline2->id_);

				driver->Bind(pipeline2->id_, 0,
					{
						cameraTransformResource1->id_
					}
				);

				driver->BindVertexBuffer(driverVertexBuffer2->id_, 0);


				driver->Draw(vertexBuffer2->vertices_.GetVerticesNumber());

			}

		}
	}


} // namespace OksEngine