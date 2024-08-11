#pragma once 

#include <Geometry.Vertex.hpp>

namespace Geometry {

	class AABB {
	public:
		Geom::Vertex3f min_;
		Geom::Vertex3f max_;
	};

}