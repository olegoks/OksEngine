#include <Geometry.Vertex.hpp>

namespace Geometry {

	Vertex3fc::Vertex3fc(const Vertex3f& position, const Math::Vector3f& color) noexcept :
		position_{ position }, color_{ color } { }


}