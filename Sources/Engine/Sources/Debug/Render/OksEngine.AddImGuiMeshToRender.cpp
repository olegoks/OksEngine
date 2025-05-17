
#include <Debug/Render/auto_OksEngine.AddImGuiMeshToRender.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {

	void AddImGuiMeshToRender::Update(
		ECS2::Entity::Id entity1Id,
		const Transform2DResource* transform2DResource,
		const TextureResource* textureResource,
		const ImGuiDriverIndexBuffer* imGuiDriverIndexBuffer,
		const ImGuiDriverVertexBuffer* imGuiDriverVertexBuffer,
		ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

		auto driver = renderDriver->driver_;

		std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

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

		driver->StartDrawing();
		driver->BindPipeline("ImGui Pipeline");
		driver->BindVertexBuffer(imGuiDriverVertexBuffer->id_, 0);
		driver->BindIndexBuffer(imGuiDriverIndexBuffer->id_, 0);
		driver->Bind(
			{ 
				transform2DResource->id_,
				textureResource->id_ 
			} );
		driver->DrawIndexed(imGuiDriverIndexBuffer->size_ / sizeof(Common::UInt16));
		driver->EndDrawing();

		//Common::Id driverMeshId = driver->DrawMesh(
		//	"ImGui Pipeline",
		//	driverVertexBuffer->id_,
		//	driverIndexBuffer->id_,
		//	shaderBindings
		//);
	}


}