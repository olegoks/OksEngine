#pragma once

#include <Math.Vector.hpp>
#include <Geometry.Common.hpp>
#include <Geometry.Vertex.hpp>
#include <Geometry.Polygon.hpp>
#include <Geometry.Texture.hpp>

namespace RAL {


	using Index16 = Common::UInt16;

	using Vector3f = Math::Vector3f;
	using Vertex3f = Geometry::Vertex3f;
	using Vertex2f = Geometry::Vertex2f;
	using Color3f = Geometry::Color3f;
	using Color4b = Geometry::Color4b;
	using Vertex3fnc = Geometry::Vertex3fnc;
	using Vertex3fc = Geometry::Vertex3fc;
	using Normal3f = Geom::Normal3f;
	using UV2f = Geom::UV2f;
	using Vertex3fnt = Geometry::Vertex3fnt;
	using Vertex2ftc = Geom::Vertex2ftc;
	//using Vertex3fn�t = Geometry::Vertex3fnct;
	//using Texture = Geometry::Texture<Geom::Color4b>;

	template<class VertexType>
	using Polygon = Geom::Polygon<VertexType>;

	using VertexType = Geom::VertexType;
	//struct Vertex3fnc {
	//	Vertex3f position_{ 0.f, 0.f, 0.f };
	//	Math::Vector3f normal_ = { 0.f, 0.f, 0.f };
	//	Math::Vector3f color_ = { 1.f, 1.f, 1.f };
	//	Vertex3fnc() noexcept = default;
	//	Vertex3fnc(const Vertex3f& position, const Vertex3f& normal, const Math::Vector3f& color) noexcept :
	//		position_{ position }, normal_{ normal }, color_{ color } { }


	//	struct Hash {
	//		[[nodiscard]]
	//		Common::UInt64 operator()(const Vertex3fnc& vertex) const noexcept {
	//			return
	//				vertex.position_.GetHash() ^
	//				vertex.normal_.GetHash() ^
	//				vertex.color_.GetHash();
	//		}
	//	};

	//	[[nodiscard]]
	//	bool operator==(const Vertex3fnc& vertex) const noexcept = default;/*
	//	{
	//		return (position_ == vertex.position_) && (normal_ == vertex.normal_) && (color_ == vertex.color_);
	//	}*/

	//};

	//struct Vertex3fc {
	//	Vertex3f position_{ 0.f, 0.f, 0.f };
	//	Math::Vector3f color_ = { 1.f, 1.f, 1.f };
	//	Vertex3fc() noexcept = default;
	//	Vertex3fc(const Vertex3f& position, const Math::Vector3f& color) noexcept :
	//		position_{ position }, color_{ color } { }
	//};


}