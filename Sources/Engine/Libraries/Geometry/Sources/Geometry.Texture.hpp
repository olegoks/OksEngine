#pragma once

#include <Geometry.Common.hpp>
#include <Common.hpp>
#include <OS.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	class Texture {
	public:

		class Id final {
		public:

			using ValueType = Common::Size;

			static const Id invalid_;

			Id() noexcept : value_{ invalid_ } { }
			Id(ValueType value) noexcept : value_{ value } {}

			operator ValueType() const noexcept {
				return value_;
			}

			Id operator++(int value) noexcept {
				return ++value_;
			}

			struct Hash {
				std::size_t operator()(const Id& id) const noexcept {
					const Id::ValueType value = id.operator size_t();
					return std::hash<Id::ValueType>{}(value);
				}
			};

			~Id() noexcept = default;

		private:
			ValueType value_;
		};

		struct CreateInfo {
			Common::Size width_ = 0;
			Common::Size height_ = 0;
			DS::Vector<Color4b> pixels_;
		};

		Texture(Texture&& moveTexture) noexcept : createInfo_{ } {

			std::swap(createInfo_, moveTexture.createInfo_);

		}

		Texture(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

		}

		[[nodiscard]]
		glm::u32vec2 GetSize() const noexcept {
			return { static_cast<Common::UInt32>(createInfo_.width_),  static_cast<Common::UInt32>(createInfo_.height_) };
		}

		[[nodiscard]]
		Common::Size GetWidth() const noexcept { return createInfo_.width_; }

		[[nodiscard]]
		Common::Size GetHeight() const noexcept { return createInfo_.height_; }

		[[nodiscard]]
		Common::Size GetPixelsNumber() const noexcept { return GetWidth() * GetHeight(); }

		template<class PixelType>
		const PixelType* GetPixels() const noexcept {
			return reinterpret_cast<const PixelType*>(createInfo_.pixels_.GetData());
		}

		~Texture() noexcept {
		};

	private:
		CreateInfo createInfo_;
	};

	[[nodiscard]]
	std::shared_ptr<Texture> CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept;

}
