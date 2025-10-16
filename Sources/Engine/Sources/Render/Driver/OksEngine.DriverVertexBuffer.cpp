#pragma once
#include <Render\Driver\auto_OksEngine.DriverVertexBuffer.hpp>

namespace OksEngine
{
	void UpdateDriverVertexBuffer::Update(ECS2::Entity::Id entity0id, const Vertices2D* vertices2D0,
		const Indices* indices0, const UVs* uVs0, DriverVertexBuffer* driverVertexBuffer0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {


		//auto* vertices = world->GetComponent<Vertices2D>(entityId);
		//auto* colors = world->GetComponent<Colors>(entityId);
		//auto* uvs = world->GetComponent<UVs>(entityId);

		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		//std::vector<RAL::Vertex2ftc> vertices2ftc;
		//vertices2ftc.reserve(vertices->vertices_.GetVerticesNumber());
		//for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
		//	RAL::Vertex2ftc vertex;
		//	vertex.position_ = vertices->vertices_[i].position_;
		//	vertex.uv_ = uvs->uvs_[i];
		//	vertex.color_ = colors->colors_[i];
		//	vertices2ftc.push_back(vertex);
		//}

		//auto driverVB = world->GetComponent<DriverVertexBuffer>(entityId);
		//const Common::Size currentVBSize = driver->GetVBSizeInBytes(driverVB->id_);
		//if (currentVBSize != vertices2ftc.size() * sizeof(RAL::Vertex2ftc)) {
		//	driver->ResizeVertexBuffer(driverVB->id_, vertices2ftc.size());
		//}
		//driver->FillVertexBuffer(driverVB->id_, 0, vertices2ftc.data(), vertices2ftc.size());


	};

	void CreateDriverVertexBuffer2D::Update(

		ECS2::Entity::Id entity0id,
		const Vertices2D* vertices2D0,
		const Indices* indices0, 
		const Colors* colors0, 
		const UVs* uVs0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {



		auto driver = renderDriver1->driver_;

		std::vector<RAL::Vertex2ftc> vertices2ftc;
		vertices2ftc.reserve(vertices2D0->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices2D0->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex2ftc vertex;
			vertex.position_ = vertices2D0->vertices_[i].position_;
			vertex.uv_ = uVs0->uvs_[i];
			vertex.color_ = colors0->colors_[i];
			vertices2ftc.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices2D0->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		driver->FillVertexBuffer(VBId, 0, vertices2ftc.data(), vertices2ftc.size());
		CreateComponent<DriverVertexBuffer>(entity0id, VBId);


	};

	void CreateDriverVertexBuffer::Update(ECS2::Entity::Id entity0id, const Vertices3D* vertices3D0,
		const Indices* indices0, const Normals* normals0, const UVs* uVs0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

		std::vector<RAL::Vertex3fnt> vertices3fnt;
		vertices3fnt.reserve(vertices3D0->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices3D0->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex3fnt vertex;
			vertex.position_ = vertices3D0->vertices_[i].position_;
			vertex.normal_ = normals0->normals_[i];
			vertex.uv_ = uVs0->uvs_[i];
			vertices3fnt.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices3D0->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = renderDriver1->driver_->CreateVertexBuffer(VBCI);
		renderDriver1->driver_->FillVertexBuffer(VBId, 0, vertices3fnt.data(), vertices3fnt.size());
		CreateComponent<DriverVertexBuffer>(entity0id, VBId);



	};

	void CreateDriverRiggedVertexBuffer::Update(
		ECS2::Entity::Id entity0id,
		const Vertices3D* vertices3D0,
		const Indices* indices0,
		const Normals* normals0,
		const UVs* uVs0,
		const VertexBones* vertexBones0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {

		std::vector<RAL::Vertex3fntbw> vertices3fntbw;
		vertices3fntbw.reserve(vertices3D0->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices3D0->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex3fntbw vertex;
			vertex.position_ = vertices3D0->vertices_[i].position_;
			vertex.normal_ = normals0->normals_[i];
			vertex.uv_ = uVs0->uvs_[i];

			const glm::u32vec4 boneIndices = vertexBones0->vertexBonesInfos_[i].boneEntityIndices_;
			vertex.boneIds_ = { boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]};

			const glm::vec4 weights = vertexBones0->vertexBonesInfos_[i].boneWeights_;
			vertex.weights_ = { weights[0], weights[1], weights[2], weights[3] };

			vertices3fntbw.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices3D0->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2_BIDUB4_WUB4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = renderDriver1->driver_->CreateVertexBuffer(VBCI);
		renderDriver1->driver_->FillVertexBuffer(VBId, 0, vertices3fntbw.data(), vertices3fntbw.size());
		CreateComponent<DriverVertexBuffer>(entity0id, VBId);

	}

} // namespace OksEngine