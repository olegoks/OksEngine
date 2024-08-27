#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr }{

}

ImmutableRenderGeometry::ImmutableRenderGeometry(const std::string& meshTag) :
	ECSComponent{ nullptr },
	meshTag_{ meshTag } {}