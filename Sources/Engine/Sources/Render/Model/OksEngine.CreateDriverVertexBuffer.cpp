
#include <Render/Model/OksEngine.CreateDriverVertexBuffer.hpp>

#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverVertexBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* vertices = world->GetComponent<Vertices3D>(entityId);
		auto* normals = world->GetComponent<Normals>(entityId);
		auto* uvs = world->GetComponent<UVs>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		std::vector<RAL::Vertex3fnt> vertices3fnt;
		vertices3fnt.reserve(vertices->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex3fnt vertex;
			vertex.position_ = vertices->vertices_[i].position_;
			vertex.normal_ = normals->normals_[i];
			vertex.uv_ = uvs->uvs_[i];
			vertices3fnt.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		driver->FillVertexBuffer(VBId, 0, vertices3fnt.data(), vertices3fnt.size());
		world->CreateComponent<DriverVertexBuffer>(entityId, VBId);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverVertexBuffer::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<Vertices3D>()
			.Include<Indices>()
			.Include<Normals>()
			.Include<UVs>()
			.Exclude<DriverVertexBuffer>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateDriverVertexBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverVertexBuffer>().GetId();
	}



	void CreateDriverVertex2DBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* vertices = world->GetComponent<Vertices2D>(entityId);
		auto* colors = world->GetComponent<Colors>(entityId);
		auto* uvs = world->GetComponent<UVs>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		std::vector<RAL::Vertex2ftc> vertices2ftc;
		vertices2ftc.reserve(vertices->vertices_.GetVerticesNumber());
		for (Common::Index i = 0; i < vertices->vertices_.GetVerticesNumber(); i++) {
			RAL::Vertex2ftc vertex;
			vertex.position_ = vertices->vertices_[i].position_;
			vertex.uv_ = uvs->uvs_[i];
			vertex.color_ = colors->colors_[i];
			vertices2ftc.push_back(vertex);
		}


		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = vertices->vertices_.GetVerticesNumber(),
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		driver->FillVertexBuffer(VBId, 0, vertices2ftc.data(), vertices2ftc.size());
		world->CreateComponent<DriverVertexBuffer>(entityId, VBId);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverVertex2DBuffer::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<Vertices2D>()
			.Include<Indices>()
			.Include<UVs>()
			.Exclude<DriverVertexBuffer>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateDriverVertex2DBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverVertex2DBuffer>().GetId();
	}
}