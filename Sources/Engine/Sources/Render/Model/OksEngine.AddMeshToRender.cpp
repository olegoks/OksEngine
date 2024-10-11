
#include <Render/Model/OksEngine.AddMeshToRender.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Common/OksEngine.Rotation.hpp>
#include <Render/Model/OksEngine.Mesh.hpp>
#include <Render/Texture/OksEngine.DriverTexture.hpp>
#include <Render/Model/OksEngine.DriverMesh.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Camera/OksEngine.Camera.hpp>
#include <Render/Driver/OksEngine.UniformBuffer.hpp>

namespace OksEngine {

	void AddMeshToRender::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position>(entityId);
		auto* rotation = world->GetComponent<Rotation>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//auto* driverCamera = world->GetComponent<DriverCamera>(secondEntityId);
		auto* driverTexture = world->GetComponent<DriverTexture>(entityId);

		auto* vertices = world->GetComponent<Vertices>(entityId);
		auto* indices = world->GetComponent<Indices>(entityId);
		auto* uvs = world->GetComponent<UVs>(entityId);
		auto* normals = world->GetComponent<Normals>(entityId);

		std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

		//CAMERA BINDING
		auto* cameraViewProjUniformBuffer = world->GetConstComponent<UniformBuffer>(secondEntityId);
		{
			RAL::Driver::ShaderBinding::Data cameraBinding{
				.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
				.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
				.uniformBufferId_ = cameraViewProjUniformBuffer->id_
			};
			shaderBindings.push_back(cameraBinding);
		}

		//TRANSFORM BINDING
		{
			const glm::mat4 transformMatrix = glm::mat4{ 1 } * rotation->GetMat() * position->GetMat();
			struct Transform {
				glm::mat4 model_;
			};

			Transform transform{ transformMatrix };
			RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
				.size_ = sizeof(Transform),
				.type_ = RAL::Driver::UniformBuffer::Type::Const
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

		std::vector<RAL::Vertex3fnt> vertices3fnt;
		vertices3fnt.reserve(vertices->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex3fnt vertex;
			vertex.position_ = vertices->vertices_[i].position_;
			vertex.normal_ = normals->normals_[i];
			vertex.uv_ = uvs->uvs_[i];
			vertices3fnt.push_back(vertex);
		}
		Common::Id driverMeshId = driver->DrawMesh(
			"Textured Pipeline",
			(const RAL::Vertex3fnt*)vertices3fnt.data(),
			vertices->vertices_.GetVerticesNumber(),
			RAL::Driver::VertexType::VF3_NF3_TF2,
			(const RAL::Index16*)indices->indices_.GetData(),
			indices->indices_.GetIndicesNumber(),
			RAL::Driver::IndexType::UI16,
			shaderBindings
		);
		world->CreateComponent<DriverMesh>(entityId, driverMeshId);
	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> AddMeshToRender::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
			.Include<Mesh2>()
			.Include<Position>()
			.Include<Rotation>()
			.Include<Mesh2>()
			.Include<Vertices>()
			.Include<Indices>()
			.Include<UVs>()
			.Include<Normals> ()
			.Include<DriverTexture>()
			.Exclude<DriverMesh>()
			, ECS::Entity::Filter{}.Include<Camera>().Include<UniformBuffer>()};
	}

	Common::TypeId AddMeshToRender::GetTypeId() const noexcept {
		return Common::TypeInfo<AddMeshToRender>().GetId();
	}

}