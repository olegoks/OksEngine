
#include <Render/OksEngine.CreateDriverModel.hpp>

#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/OksEngine.DriverModel.hpp>
#include <Render/OksEngine.DriverCamera.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.Mesh.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	//void CreateDriverModel::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
	//	auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
	//	auto* meshComponent = world->GetComponent<Mesh>(entityId);
	//	auto* position = world->GetComponent<Position>(entityId);
	//	auto driver = GetContext().GetRenderSubsystem()->GetDriver();
	//	auto& model = GetContext().GetModelStorage()->GetModel(meshComponent->modelId_);


	//	auto* driverCamera = world->GetComponent<DriverCamera>(secondEntityId);

	//	std::map<Geom::Texture::Id, RAL::Texture::Id> createdTextures;
	//	std::vector<DriverModel::Mesh> driverMeshs;


	//	model.ForEachMesh([&](ModelStorage::Model::Mesh& mesh) {
	//		if (mesh.textureStorageId_.IsInvalid()) return true;

	//		std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings;

	//		//CAMERA BINDING
	//		{
	//			RAL::Driver::ShaderBinding::Data cameraBinding{
	//				.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
	//				.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
	//				.uniformBufferId_ = driverCamera->matricesBuffer_
	//			};
	//			shaderBindings.push_back(cameraBinding);
	//		}

	//		//TRANSFORM BINDING
	//		{
	//			struct Transform {
	//				glm::mat4 model_;
	//			};

	//			Transform transform{ position->GetMat() };
	//			RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
	//				.size_ = sizeof(Transform),
	//				.type_ = RAL::Driver::UniformBuffer::Type::Const
	//			};
	//			RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);

	//			RAL::Driver::ShaderBinding::Data transformBinding{
	//				.type_ = RAL::Driver::ShaderBinding::Type::Uniform,
	//				.stage_ = RAL::Driver::ShaderBinding::Stage::VertexShader,
	//				.uniformBufferId_ = ubId
	//			};
	//			shaderBindings.push_back(transformBinding);
	//		}

	//		//TEXTURE BINDING
	//		{
	//			RAL::Texture::Id driverTextureId;
	//			if (createdTextures.contains(mesh.textureStorageId_)) {
	//				const RAL::Texture::Id textureId = createdTextures[mesh.textureStorageId_];
	//				driverTextureId = textureId;
	//			}
	//			else {
	//				auto& texture = GetContext().GetTextureStorage()->Get(mesh.textureStorageId_);
	//				std::vector<RAL::Color4b> texturePixels{ texture.GetPixels<RAL::Color4b>(), texture.GetPixels<RAL::Color4b>() + texture.GetPixelsNumber() };
	//				RAL::Texture::CreateInfo textureCreateInfo{
	//					.name_ = "",
	//					.pixels_ = texturePixels,
	//					.size_ = texture.GetSize()
	//				};
	//				RAL::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);
	//				driverTextureId = textureId;
	//				createdTextures[mesh.textureStorageId_] = textureId;
	//			}

	//			OS::AssertMessage(!driverTextureId.IsInvalid(), "");

	//			RAL::Driver::ShaderBinding::Data textureBinding{
	//				.type_ = RAL::Driver::ShaderBinding::Type::Sampler,
	//				.stage_ = RAL::Driver::ShaderBinding::Stage::FragmentShader,
	//				.textureId_ = driverTextureId
	//			};

	//			shaderBindings.push_back(textureBinding);
	//		}

	//		const Geom::Mesh& geomMesh = GetContext().GetMeshStorage()->Get(mesh.meshId_);
	//		auto vertices = Geometry::GetVertexCloud3fnt(geomMesh);
	//		//glm::mat4 localToWorld = position->GetTranslateMat();

	//		Common::Id driverModelId = driver->DrawMesh(
	//			"Textured Pipeline",
	//			(const RAL::Vertex3fnt*)vertices.GetData(),
	//			vertices.GetVerticesNumber(),
	//			RAL::Driver::VertexType::VF3_NF3_TF2,
	//			(const RAL::Index16*)geomMesh.indices_.GetData(),
	//			geomMesh.indices_.GetIndicesNumber(),
	//			RAL::Driver::IndexType::UI16,
	//			shaderBindings
	//		);

	//		DriverModel::Mesh driverMesh{
	//			.id_ = driverModelId,
	//			.shaderBindings_ = shaderBindings
	//		};

	//		driverMeshs.push_back(driverMesh);

	//		return true;

	//		});

	//	OS::AssertMessage(!driverMeshs.empty(), "Model doesnt contain any mesh.");

	//	world->CreateComponent<DriverModel>(entityId, driverMeshs);

	//}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverModel::GetFilter() const noexcept {
	//	return { ECS::Entity::Filter{}
	//		.Include<Mesh>()
	//		.Exclude<DriverModel>(), ECS::Entity::Filter{}.Include<DriverCamera>() };
	//}

}