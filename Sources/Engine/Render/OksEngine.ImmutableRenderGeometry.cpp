#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Render\OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr } {

	}

	ImmutableRenderGeometry::ImmutableRenderGeometry(const std::string& meshTag) :
		ECSComponent{ nullptr },
		meshTag_{ meshTag } {}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderMesh::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>().Include<Position>(), ECS::Entity::Filter{}.ExcludeAll() };
	}



	void RenderMesh::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* renderGeom = world->GetComponent<ImmutableRenderGeometry>(entityId);
		auto* meshComponent = world->GetComponent<Mesh>(entityId);
		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		auto& mesh = GetContext().GetGeomStorage()->Get(meshComponent->meshId_);
		auto vertices = Geometry::GetVertexCloud3fnt(mesh);
		driver->DrawIndexed(
			position->GetTranslateMat(),
			(const RAL::Vertex3fnt*)vertices.GetData(),
			vertices.GetVerticesNumber(),
			(const RAL::Index16*)mesh.indices_.GetData(),
			mesh.indices_.GetIndicesNumber(), 0);
	}

}