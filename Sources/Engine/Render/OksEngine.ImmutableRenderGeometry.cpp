#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <Common/OksEngine.Position.hpp>

namespace OksEngine {

	ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr } {

	}

	ImmutableRenderGeometry::ImmutableRenderGeometry(const std::string& meshTag) :
		ECSComponent{ nullptr },
		meshTag_{ meshTag } {}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> RenderMesh::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Mesh>().Include<ImmutableRenderGeometry>().Include<Position>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}