#pragma once

#include <functional>

#include <Common.Types.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	template<Common::Size size, class Type>
	using Vertex = Math::Vector<size, Type>;

	using Vertex3f = Vertex<3, float>;


	struct Vertex3fc {
		Vertex3f position_{ 0.f, 0.f, 0.f };
		Math::Vector3f color_ = { 1.f, 1.f, 1.f };
		Vertex3fc() noexcept = default;
		Vertex3fc(const Vertex3f& position, const Math::Vector3f& color) noexcept;
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

	struct Vertex3fn {
		Vertex3f position_{ 0.f, 0.f, 0.f };
		Math::Vector3f normal_ = { 1.f, 1.f, 1.f };
		Vertex3fn() noexcept = default;
		Vertex3fn(const Vertex3f& position, const Math::Vector3f& normal) noexcept :
			position_{ position }, normal_{ normal } { }

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
		Math::Vector3f color_ = { 1.f, 1.f, 1.f };
		Vertex3fnc() noexcept = default;
		Vertex3fnc(const Vertex3f& position, const Vertex3f& normal, const Math::Vector3f& color) noexcept :
			Vertex3fn{ position, normal }, color_ { color } { }


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

	struct Vertex3fnt {
		Vertex3f position_{ 0.f, 0.f, 0.f };
		Math::Vector3f normal_ = { 0.f, 0.f, 0.f };
		Math::Vector2f texel_ = { 0.f, 0.f };
		Vertex3fnt() noexcept = default;
		Vertex3fnt(const Vertex3f& position, const Vertex3f& normal, const Math::Vector2f& texel) noexcept :
			position_{ position }, normal_{ normal }, texel_{ texel } { }


		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fnt& vertex) const noexcept {
				return
					vertex.position_.GetHash() ^
					vertex.normal_.GetHash() ^
					vertex.texel_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fnt& vertex) const noexcept {
			return (position_ == vertex.position_) && (normal_ == vertex.normal_) && (texel_ == vertex.texel_);
		}

	};

	struct Vertex3fnct {
		Vertex3f position_{ 0.f, 0.f, 0.f };
		Math::Vector3f normal_ = { 0.f, 0.f, 0.f };
		Math::Vector3f color_ = { 1.f, 1.f, 1.f };
		Math::Vector2f texel_ = { 0.f, 0.f };
		Vertex3fnct() noexcept = default;
		Vertex3fnct(
			const Vertex3f& position,
			const Vertex3f& normal,
			const Math::Vector3f& color,
			const Math::Vector2f& texel) noexcept :
			position_{ position },
			normal_{ normal },
			color_{ color },
			texel_ { texel } { }


		struct Hash {
			[[nodiscard]]
			Common::UInt64 operator()(const Vertex3fnct& vertex) const noexcept {
				return
					vertex.position_.GetHash() ^
					vertex.normal_.GetHash() ^
					vertex.color_.GetHash() ^
					vertex.texel_.GetHash();
			}
		};

		[[nodiscard]]
		bool operator==(const Vertex3fnct& vertex) const noexcept {
			return 
				(position_ == vertex.position_) &&
				(normal_ == vertex.normal_) &&
				(color_ == vertex.color_) &&
				(texel_ == vertex.texel_);
		}

	};


}