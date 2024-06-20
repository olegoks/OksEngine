#pragma once

#include <functional>

#include <Geometry.Common.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	enum class VertexType : Common::UInt64 {
		V3f = 0,
		V3f_C3f,
		V3f_N3f_C3f,
		V3f_N3f_T3f,
		Undefined
	};

	template<Common::Size size, class BaseType>
	struct Vertex {

		Vertex() noexcept : position_{ 0.f , 0.f , 0.f }{ }

		Vertex(const Math::Vector<size, BaseType>& position) noexcept :
			position_{ position } {}

		Vertex(BaseType x, BaseType y, BaseType z) noexcept requires(size == 3) :
			position_{ x, y, z } {}

		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex& vertex) const noexcept {
				return vertex.position_.GetHash();
			}
		};

		[[nodiscard]]
		Common::UInt64 GetHash() const noexcept {
			return position_.GetHash();
		}

		[[nodiscard]]
		bool operator==(const Vertex& vertex) const noexcept {
			return position_ == vertex.position_;
		}

		Vector<size, BaseType> position_{ 0, 0, 0 };
	};

	using Vertex3f = Vertex<3, float>;

	struct Vertex3fc : public Vertex3f {
		Color3f color_ = { 1.f, 1.f, 1.f };
		Vertex3fc() noexcept = default;
		Vertex3fc(const Vertex3f& position, const Color3f& color) noexcept :
			Vertex3f{ position }, color_{ color } { }

		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fc& vertex) const noexcept {
				return
					vertex.position_.GetHash() ^
					vertex.color_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fc& vertex) const noexcept {
			return (position_ == vertex.position_) && (color_ == vertex.color_);
		}

	};

	struct Vertex3fn : public Vertex3f {
		Normal3f normal_ = { 1.f, 1.f, 1.f };
		Vertex3fn() noexcept = default;
		Vertex3fn(const Vertex3f& position, const Normal3f& normal) noexcept :
			Vertex3f{ position }, normal_{ normal } { }

		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fn& vertex) const noexcept {
				return
					vertex.position_.GetHash() ^
					vertex.normal_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fn& vertex) const noexcept {
			return (position_ == vertex.position_) && (normal_ == vertex.normal_);
		}

	};

	struct Vertex3fnc : public Vertex3fn {
		Color3f color_ = { 1.f, 1.f, 1.f };
		Vertex3fnc() noexcept = default;
		Vertex3fnc(const Vertex3f& position, const Math::Vector3f& normal, const Math::Vector3f& color) noexcept :
			Vertex3fn{ position, normal }, color_ { color } { }
		Vertex3fnc(const Vertex3fc& vertex3fc, const Normal3f& normal) noexcept :
			Vertex3fn{ vertex3fc.position_, normal }, color_{ vertex3fc.color_ } { }


		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fnc& vertex) const noexcept {
				return 
					vertex.position_.GetHash() ^
					vertex.normal_.GetHash() ^
					vertex.color_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fnc& vertex) const noexcept {
			return (position_ == vertex.position_) && (normal_ == vertex.normal_) && (color_ == vertex.color_);
		}

	};

	struct Vertex3fnt : public Vertex3fn {
		UV2f uv_ = { 0.f, 0.f };
		Vertex3fnt() noexcept = default;
		Vertex3fnt(const Vertex3f& position, const Normal3f& normal, const UV2f& texel) noexcept :
			Vertex3fn{ position , normal }, uv_{ texel } { }


		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fnt& vertex) const noexcept {
				return
					vertex.position_.GetHash() ^
					vertex.normal_.GetHash() ^
					vertex.uv_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fnt& vertex) const noexcept {
			return (position_ == vertex.position_) && (normal_ == vertex.normal_) && (uv_ == vertex.uv_);
		}

	};

	//struct Vertex3fnct {
	//	Vertex3f position_{ 0.f, 0.f, 0.f };
	//	Math::Vector3f normal_ = { 0.f, 0.f, 0.f };
	//	Math::Vector3f color_ = { 1.f, 1.f, 1.f };
	//	Math::Vector2f texel_ = { 0.f, 0.f };
	//	Vertex3fnct() noexcept = default;
	//	Vertex3fnct(
	//		const Vertex3f& position,
	//		const Math::Vector3f& normal,
	//		const Math::Vector3f& color,
	//		const Math::Vector2f& texel) noexcept :
	//		position_{ position },
	//		normal_{ normal },
	//		color_{ color },
	//		texel_ { texel } { }


	//	struct Hash {
	//		[[nodiscard]]
	//		Common::UInt64 operator()(const Vertex3fnct& vertex) const noexcept {
	//			return
	//				vertex.position_.GetHash() ^
	//				vertex.normal_.GetHash() ^
	//				vertex.color_.GetHash() ^
	//				vertex.texel_.GetHash();
	//		}
	//	};

	//	[[nodiscard]]
	//	bool operator==(const Vertex3fnct& vertex) const noexcept {
	//		return 
	//			(position_ == vertex.position_) &&
	//			(normal_ == vertex.normal_) &&
	//			(color_ == vertex.color_) &&
	//			(texel_ == vertex.texel_);
	//	}

	//};


}

namespace Geom = Geometry;