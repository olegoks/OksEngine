
#include <Debug/Render/OksEngine.AddImGuiMeshToRender.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Common/auto_OksEngine.Position2D.hpp>
#include <Common/auto_OksEngine.Scale2D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Render/Model/OksEngine.Mesh.hpp>
#include <Render/Texture/OksEngine.DriverTexture.hpp>
#include <Render/Model/OksEngine.DriverMesh.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Camera/OksEngine.Camera.hpp>
#include <Render/Driver/OksEngine.UniformBuffer.hpp>
#include <Render/Driver/OksEngine.DriverIndexBuffer.hpp>
#include <Render/Driver/OksEngine.DriverVertexBuffer.hpp>

namespace OksEngine {

	void AddImGuiMeshToRender::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position2D>(entityId);
		auto* scale = world->GetComponent<Scale2D>(entityId);
		//auto* rotation = world->GetComponent<Rotation3D>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//auto* driverCamera = world->GetComponent<DriverCamera>(secondEntityId);
		auto* driverTexture = world->GetComponent<DriverTexture>(entityId);

		auto* vertices = world->GetComponent<DriverVertexBuffer>(entityId);
		auto* indices = world->GetComponent<DriverIndexBuffer>(entityId);
		//auto* uvs = world->GetComponent<UVs>(entityId);
		//auto* normals = world->GetComponent<Normals>(entityId);

		std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

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

		//TRANSFORM BINDING
		{
			struct Transform {
				glm::vec2 scale_;
				glm::vec2 translate_;
			};

			Transform transform{ 
				glm::vec2{ scale->x_, scale->y_ },
				glm::vec2{ position->x_, position->y_ }
			};
			RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
				.size_ = sizeof(Transform),
				.type_ = RAL::Driver::UniformBuffer::Type::Mutable
			};
			RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);
			driver->FillUniformBuffer(ubId, &transform);
			RAL::Driver::ShaderBinding::Data transformBinding{
				.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
				.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
				.uniformBufferId_ = ubId
			};
			shaderBindings.push_back(transformBinding);
		}

		//TEXTURE BINDING
		{

			RAL::Driver::ShaderBinding::Data textureBinding{
				.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
				.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
				.textureId_ = driverTexture->driverTextureId_
			};

			shaderBindings.push_back(textureBinding);
		}

		//std::vector<RAL::Vertex3fnt> vertices3fnt;
		//vertices3fnt.reserve(vertices->vertices_.GetVerticesNumber());
		//for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
		//	RAL::Vertex3fnt vertex;
		//	vertex.position_ = vertices->vertices_[i].position_;
		//	vertex.normal_ = normals->normals_[i];
		//	vertex.uv_ = uvs->uvs_[i];
		//	vertices3fnt.push_back(vertex);
		//}
		Common::Id driverMeshId = driver->DrawMesh(
			"ImGui Pipeline",
			/*(const RAL::Vertex3fnt*)vertices3fnt.data(),
			vertices->vertices_.GetVerticesNumber(),
			RAL::Driver::VertexType::VF3_NF3_TF2,*/
			vertices->id_,
			indices->id_,
			/*(const RAL::Index16*)indices->indices_.GetData(),
			indices->indices_.GetIndicesNumber(),
			RAL::Driver::IndexType::UI16,*/
			shaderBindings
		);
		//world->CreateComponent<DriverMesh>(entityId, driverMeshId);
	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> AddImGuiMeshToRender::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<Mesh2>()
			.Include<Position2D>()
			.Include<Scale2D>()
			.Include<Vertices2D>()
			.Include<Indices>()
			.Include<UVs>()
			.Include<Colors>()
			.Include<DriverTexture>()
			.Include<DriverIndexBuffer>()
			.Include<DriverVertexBuffer>()
			//.Exclude<DriverMesh>()
			, ECS::Entity::Filter{}.Include<Camera>().Include<UniformBuffer>() };
		return filter;
	}

	Common::TypeId AddImGuiMeshToRender::GetTypeId() const noexcept {
		return Common::TypeInfo<AddImGuiMeshToRender>().GetId();
	}

}