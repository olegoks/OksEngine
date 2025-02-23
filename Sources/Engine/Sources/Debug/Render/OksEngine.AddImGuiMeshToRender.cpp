
#include <Debug/Render/auto_OksEngine.AddImGuiMeshToRender.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {

	void AddImGuiMeshToRender::Update(
		ECS2::Entity::Id entity1Id,
		const Position2D* position2D,
		const Scale2D* scale2D,
		const Vertices2D* vertices2D,
		const Indices* indices,
		const UVs* uVs,
		const Colors* colors,
		const DriverTexture* driverTexture,
		const DriverIndexBuffer* driverIndexBuffer,
		const DriverVertexBuffer* driverVertexBuffer,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		//auto* scale = world->GetComponent<Scale2D>(entityId);
		////auto* rotation = world->GetComponent<Rotation3D>(entityId);
		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		////auto* driverCamera = world->GetComponent<DriverCamera>(secondEntityId);
		//auto* driverTexture = world->GetComponent<DriverTexture>(entityId);

		//auto* vertices = world->GetComponent<DriverVertexBuffer>(entityId);
		//auto* indices = world->GetComponent<DriverIndexBuffer>(entityId);
		////auto* uvs = world->GetComponent<UVs>(entityId);
		////auto* normals = world->GetComponent<Normals>(entityId);

		//std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

		////CAMERA BINDING
		//auto* cameraViewProjUniformBuffer = world->GetConstComponent<UniformBuffer>(secondEntityId);
		//{
		//	RAL::Driver::ShaderBinding::Data cameraBinding{
		//		.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
		//		.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
		//		.uniformBufferId_ = cameraViewProjUniformBuffer->id_
		//	};
		//	shaderBindings.push_back(cameraBinding);
		//}

		////TRANSFORM BINDING
		//{
		//	struct Transform {
		//		glm::vec2 scale_;
		//		glm::vec2 translate_;
		//	};

		//	Transform transform{ 
		//		glm::vec2{ scale2D->x_, scale2D->y_ },
		//		glm::vec2{ position2D->x_, position2D->y_ }
		//	};
		//	RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
		//		.size_ = sizeof(Transform),
		//		.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		//	};
		//	RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);
		//	driver->FillUniformBuffer(ubId, &transform);
		//	RAL::Driver::ShaderBinding::Data transformBinding{
		//		.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
		//		.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
		//		.uniformBufferId_ = ubId
		//	};
		//	shaderBindings.push_back(transformBinding);
		//}

		////TEXTURE BINDING
		//{

		//	RAL::Driver::ShaderBinding::Data textureBinding{
		//		.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
		//		.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
		//		.textureId_ = driverTexture->driverTextureId_
		//	};

		//	shaderBindings.push_back(textureBinding);
		//}

		//Common::Id driverMeshId = driver->DrawMesh(
		//	"ImGui Pipeline",
		//	/*(const RAL::Vertex3fnt*)vertices3fnt.data(),
		//	vertices->vertices_.GetVerticesNumber(),
		//	RAL::Driver::VertexType::VF3_NF3_TF2,*/
		//	driverVertexBuffer->id_,
		//	driverIndexBuffer->id_,
		//	/*(const RAL::Index16*)indices->indices_.GetData(),
		//	indices->indices_.GetIndicesNumber(),
		//	RAL::Driver::IndexType::UI16,*/
		//	shaderBindings
		//);
		//world->CreateComponent<DriverMesh>(entityId, driverMeshId);
	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> AddImGuiMeshToRender::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<Mesh2>()
	//		.Include<Position2D>()
	//		.Include<Scale2D>()
	//		.Include<Vertices2D>()
	//		.Include<Indices>()
	//		.Include<UVs>()
	//		.Include<Colors>()
	//		.Include<DriverTexture>()
	//		.Include<DriverIndexBuffer>()
	//		.Include<DriverVertexBuffer>()
	//		//.Exclude<DriverMesh>()
	//		, ECS::Entity::Filter{}.Include<Camera>().Include<UniformBuffer>() };
	//	return filter;
	//}

}