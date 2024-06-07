#pragma once

#include <Geometry.VertexCloud.hpp>

namespace Geometry {


	template<class VertexType>
	class Polygon : public VertexCloud<VertexType> {
	public:
		Polygon(const VertexType& a, const VertexType& b, const VertexType& c) noexcept {
			Add(a);
			Add(b);
			Add(c);
		}


	private:
	};
}