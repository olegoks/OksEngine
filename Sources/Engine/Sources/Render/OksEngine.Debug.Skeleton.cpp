#pragma once
#include <Render\auto_OksEngine.Debug.Skeleton.hpp>

namespace OksEngine
{
	void CreateDebugDrawBonesPipeline::Update(
		ECS2::Entity::Id entity0id, const DebugDrawBonesRenderer* debugDrawBonesRenderer0,
		ECS2::Entity::Id entity1id, const ResourceSystem* resourceSystem1, ECS2::Entity::Id entity2id,
		RenderDriver* renderDriver2, const RenderPass* renderPass2) {


		auto driver = renderDriver2->driver_;

		Resources::ResourceData imguiVertexShaderResource
			= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/debugLines.vert");
		Resources::ResourceData imguiFragmentShaderResource
			= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/debugLines.frag");

		std::string imguiVertexShader{ imguiVertexShaderResource.GetData<Common::Byte>(), imguiVertexShaderResource.GetSize() };
		std::string imguiFragmentShader{ imguiFragmentShaderResource.GetData<Common::Byte>(), imguiFragmentShaderResource.GetSize() };


		RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "LinesVertexShader",
			.type_ = RAL::Driver::Shader::Type::Vertex,
			.code_ = imguiVertexShader
		};
		auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

		RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "LinesFragmentShader",
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

		//RAL::Driver::Shader::Binding::Layout transformBinding{
		//	.binding_ = 0,
		//	.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
		//	.stage_ = RAL::Driver::Shader::Stage::VertexShader
		//};

		//RAL::Driver::Shader::Binding::Layout samplerBinding{
		//	.binding_ = 0,
		//	.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
		//	.stage_ = RAL::Driver::Shader::Stage::FragmentShader
		//};

		//shaderBindings.push_back(transformBinding);
		//shaderBindings.push_back(samplerBinding);

		RAL::Driver::Pipeline::CI pipelineCI{
			.name_ = "Debug Draw Bones",
			.renderPassId_ = renderPass2->rpId_,
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::Pipeline::Topology::LineList,
			.vertexType_ = RAL::Driver::VertexType::VF3_CF4,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::None,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = false,
			.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always

		};

		const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

		CreateComponent<DebugDrawBonesPipeline>(entity0id, pipelineId);

	};

	void AddBoneVertices::Update(
		ECS2::Entity::Id entity0id,
		const DebugDrawBonesRenderer* debugDrawBonesRenderer0,
		Vertices3D* vertices3D0,

		ECS2::Entity::Id entity1id,
		const BoneNode* boneNode1,
		const ModelNode* modelNode1,
		const WorldPosition3D* worldPosition3D1,
		const ChildModelNodeEntities* childModelNodeEntities1) {


		const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities1->childEntityIds_;

		const Common::Size childModelNodeEntitiesNumber = childEntityIds.size();

		vertices3D0->vertices_.Reserve(childModelNodeEntitiesNumber);

		for (Common::Index i = 0; i < childModelNodeEntitiesNumber; i++) {
			vertices3D0->vertices_.Add({ worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_ });
			const ECS2::Entity::Id childEntityId = childEntityIds[i];
			auto* worldPosition3D = GetComponent<WorldPosition3D>(childEntityId);
			vertices3D0->vertices_.Add({ worldPosition3D->x_, worldPosition3D->y_, worldPosition3D->z_ });
		}

	}

	void CreateDebugDrawBonesDriverVertexBuffer::Update(ECS2::Entity::Id entity0id,
		const DebugDrawBonesRenderer* debugDrawBonesRenderer0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {



		auto driver = renderDriver1->driver_;

		const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = capacity,
			.vertexType_ = RAL::Driver::VertexType::VF3_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Mutable
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		CreateComponent<DebugDrawBonesDriverVertexBuffer>(entity0id, VBId, capacity, 0);

	};

	void UpdateDebugDrawBonesDriverVertexBuffer::Update(
		ECS2::Entity::Id entity0id,
		RenderDriver* renderDriver0,
		ECS2::Entity::Id entity1id,
		const DebugDrawBonesRenderer* debugDrawBonesRenderer1,
		const Vertices3D* vertices3D1,
		DebugDrawBonesDriverVertexBuffer* debugDrawBonesDriverVertexBuffer1) {

		Geom::VertexCloud<Geom::Vertex3fc> vertices;
		for (Common::Index i = 0; i < vertices3D1->vertices_.GetVerticesNumber(); i++) {
			Geom::Vertex3fc vertex;
			vertex.position_ = {
					vertices3D1->vertices_[i].position_.GetX(),
					vertices3D1->vertices_[i].position_.GetY(),
					vertices3D1->vertices_[i].position_.GetZ() };
			vertex.color_ = Geom::Color3f{ 0.0, 0.0, 0.0 };

			vertices.Add(vertex);
		}

		auto driver = renderDriver0->driver_;

		if (vertices.GetVerticesNumber() > 0) {
			driver->FillVertexBuffer(
				debugDrawBonesDriverVertexBuffer1->id_,
				0,
				vertices.GetData(),
				vertices.GetVerticesNumber());
			debugDrawBonesDriverVertexBuffer1->size_ = vertices.GetSizeInBytes();
		}
	}

	void CreateDrawBonesRenderer::Update() {


		return;

		const ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<DebugDrawBonesRenderer>(entityId);
		CreateComponent<Vertices3D>(entityId);
	};

	void DebugDrawBones::Update(
		ECS2::Entity::Id entity0id,
		const DebugDrawBonesRenderer* debugDrawBonesRenderer0,
		const DebugDrawBonesPipeline* debugDrawBonesPipeline0,
		const DebugDrawBonesDriverVertexBuffer* debugDrawBonesDriverVertexBuffer0,
		
		ECS2::Entity::Id entity1id,
		const Camera* camera1, 
		const Active* active1,
		const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
		const CameraTransformResource* cameraTransformResource1,
		
		ECS2::Entity::Id entity2id,
		RenderDriver* renderDriver2) {

		auto driver = renderDriver2->driver_;

		driver->SetViewport(0, 0, 2560, 1440);
		driver->SetScissor(0, 0, 2560, 1440);
		driver->BindPipeline(debugDrawBonesPipeline0->id_);
		driver->BindVertexBuffer(debugDrawBonesDriverVertexBuffer0->id_, 0);
#pragma region Assert
		ASSERT_FMSG(debugDrawBonesDriverVertexBuffer0->size_ % 2 == 0, "");
#pragma endregion
		driver->Bind(debugDrawBonesPipeline0->id_,
			{
				cameraTransformResource1->id_,
			});
		driver->Draw(debugDrawBonesDriverVertexBuffer0->size_);

	};

	void ClearDebugDrawBonesVertexBuffer::Update(
		ECS2::Entity::Id entity0id, 
		const DebugDrawBonesRenderer* debugDrawBonesRenderer0,
		Vertices3D* vertices3D0) {

		vertices3D0->vertices_.Clear();

	}

} // namespace OksEngine