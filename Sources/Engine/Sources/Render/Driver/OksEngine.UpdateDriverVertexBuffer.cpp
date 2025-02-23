
#include <Render/Driver/auto_OksEngine.UpdateDriverVertexBuffer.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void UpdateDriverVertexBuffer::Update(
		ECS2::Entity::Id entityId,
		const Vertices2D* vertices2D,
		const Indices* indices,
		const UVs* uVs,
		const DriverVertexBuffer* driverVertexBuffer) {

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

	}/*
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateDriverVertexBuffer::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Vertices2D>()
			.Include<Indices>()
			.Include<UVs>()
			.Include<DriverVertexBuffer>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateDriverVertexBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverVertexBuffer>().GetId();
	}*/

}