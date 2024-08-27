#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

namespace OksEngine {

	ImmutableRenderGeometry::ImmutableRenderGeometry() : ECSComponent{ nullptr } {

	}

	ImmutableRenderGeometry::ImmutableRenderGeometry(const std::string& meshTag) :
		ECSComponent{ nullptr },
		meshTag_{ meshTag } {}

}