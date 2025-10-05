#pragma once
#include <Editor\auto_OksEngine.Editor.InfiniteGrid.hpp>

namespace OksEngine
{
	namespace Editor
	{
		namespace InfiniteGrid
		{
		}

	} // namespace Editor

	namespace Editor
	{
		namespace InfiniteGrid
		{
			void CreatePipeline::Update(
				ECS2::Entity::Id entity0id,
				const ResourceSystem* resourceSystem0,

				ECS2::Entity::Id entity1id,
				const RenderDriver* renderDriver1,
				const RenderPass* renderPass1) {


				Resources::ResourceData vertexShaderResource = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/editorInfiniteGrid.vert");
				Resources::ResourceData fragmentShaderResource = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/editorInfiniteGrid.frag");

				std::string vertexShaderCode{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
				std::string fragmentShaderCode{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };


				RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
					.name_ = "EditorInfiniteGridVertexShader",
					.type_ = RAL::Driver::Shader::Type::Vertex,
					.code_ = vertexShaderCode
				};
				auto vertexShader = renderDriver1->driver_->CreateShader(vertexShaderCreateInfo);

				RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
					.name_ = "EditorInfiniteGridFragmentShader",
					.type_ = RAL::Driver::Shader::Type::Fragment,
					.code_ = fragmentShaderCode
				};
				auto fragmentShader = renderDriver1->driver_->CreateShader(fragmentShaderCreateInfo);

				std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

				std::vector<RAL::Driver::PushConstant> pushConstants;
				{
					RAL::Driver::PushConstant pushConstant{
						.shaderStage_ = RAL::Driver::Shader::Stage::VertexShader,
						.offset_ = 0,
						.size_ = sizeof(Editor::InfiniteGrid::CameraPushConstant)
					};
					pushConstants.emplace_back(pushConstant);
				}


				auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
				{
					multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8;
				}


				RAL::Driver::Pipeline::CI pipelineCI{
					.name_ = "Infinite grid Pipeline",
					.renderPassId_ = renderPass1->rpId_,
					.vertexShader_ = vertexShader,
					.fragmentShader_ = fragmentShader,
					.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
					.vertexType_ = RAL::Driver::VertexType::Undefined,
					.indexType_ = RAL::Driver::IndexType::Undefined,
					.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
					.cullMode_ = RAL::Driver::CullMode::Back,
					.pushConstants_ = pushConstants,
					.shaderBindings_ = shaderBindings,
					.enableDepthTest_ = true,
					.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
					.multisamplingInfo_ = multisamplingInfo

				};

				const RAL::Driver::Pipeline::Id pipelineId = renderDriver1->driver_->CreatePipeline(pipelineCI);

				CreateComponent<Pipeline>(entity1id, pipelineId);
			};
		}

	} // namespace Editor

	namespace Editor
	{
		namespace InfiniteGrid
		{
			void AddToRender::Update(
				ECS2::Entity::Id entity0id,
				RenderDriver* renderDriver0,
				const RenderPass* renderPass0,
				const Editor::InfiniteGrid::Pipeline* pipeline0,
				
				ECS2::Entity::Id entity1id, 
				const Camera* camera1,
				const Direction3D* direction3D1,
				const WorldPosition3D* worldPosition3D1,
				const Up3D* up3D1,
				const ZNear* zNear1, 
				const ZFar* zFar1,
				const Width* width1,
				const Height* height1,
				const Active* active1) {

				const glm::mat4 view = glm::lookAt(
					glm::vec3(worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_),
					glm::vec3(worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_) + glm::vec3(direction3D1->x_, direction3D1->y_, direction3D1->z_),
					glm::vec3(up3D1->x_, up3D1->y_, up3D1->z_)
				);

				glm::mat4 proj = glm::perspective(
					glm::radians(45.0f),
					width1->value_ / (float)height1->value_,
					zNear1->value_, zFar1->value_);

				proj[1][1] *= -1;

				CameraPushConstant cameraInfo{
					view,
					proj,
					{ worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_, 0.0f }
				};

				auto driver = renderDriver0->driver_;

				driver->BindPipeline(pipeline0->pipelineId_);

				driver->PushConstants(
					pipeline0->pipelineId_,
					RAL::Driver::Shader::Stage::VertexShader,
					sizeof(CameraPushConstant),
					&cameraInfo);
				
				driver->Draw(6);

			};

		}

	} // namespace Editor

} // namespace OksEngine