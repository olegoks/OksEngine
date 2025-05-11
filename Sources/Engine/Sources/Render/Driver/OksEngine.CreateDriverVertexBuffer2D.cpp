#include <Render\Driver\auto_OksEngine.CreateDriverVertexBuffer2D.hpp>

namespace OksEngine {

	void CreateDriverVertexBuffer2D::Update(
		ECS2::Entity::Id entity1Id, const Vertices2D* vertices2D,
		const Indices* indices, const Colors* colors, const UVs* uVs,
		ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

		auto driver = renderDriver->driver_;

		std::vector<RAL::Vertex2ftc> vertices2ftc;
		vertices2ftc.reserve(vertices2D->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices2D->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex2ftc vertex;
			vertex.position_ = vertices2D->vertices_[i].position_;
			vertex.uv_ = uVs->uvs_[i];
			vertex.color_ = colors->colors_[i];
			vertices2ftc.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices2D->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		driver->FillVertexBuffer(VBId, 0, vertices2ftc.data(), vertices2ftc.size());
		CreateComponent<DriverVertexBuffer>(entity1Id, VBId);


	};
}