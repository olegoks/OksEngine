export module OksEngine.RAL.Common;

import OksEngine.Math.Vector;
import OksEngine.Common;
import OksEngine.Geometry.Vertex;
import OksEngine.Geometry.Texture;

namespace RAL {


	using Index16 = Common::UInt16;

	using Vertex3f = Math::Vector3f;
	using Vector3f = Math::Vector3f;
	using Color = Math::Vector3f;

	using Vertex3fnc = Geometry::Vertex3fnc;
	using Vertex3fc = Geometry::Vertex3fc;

	using Vertex3fnt = Geometry::Vertex3fnt;
	using Vertex3fnñt = Geometry::Vertex3fnct;
	using Texture = Geometry::Texture;
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