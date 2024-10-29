#pragma once

#include <Geometry.Common.hpp>
#include <Common.hpp>
#include <OS.hpp>
#include <Math.Vector.hpp>
#include <Common.Identifier.hpp>

namespace Geometry {

	class Texture {
	public:

		using Id = Common::Id;

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

		const DS::Vector<Color4b>& GetPixelsArray() const noexcept {
			return createInfo_.pixels_;
		}

		~Texture() noexcept { };

	private:
		CreateInfo createInfo_;
	};

	[[nodiscard]]
	Texture CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept;
	[[nodiscard]]
	Texture CreateTexture(Color4b* pixels, Common::Size width, Common::Size height) noexcept;
}
