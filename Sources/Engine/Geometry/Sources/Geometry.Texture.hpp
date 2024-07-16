#pragma once

#include <Geometry.Common.hpp>
#include <Common.hpp>
#include <OS.hpp>
#include <Math.Vector.hpp>

namespace Geometry {

	template<class PixelType = Color4b>
	class Texture {
	public:

		enum class Format : Common::UInt64 {
			RGBA32
		};

		struct CreateInfo {
			Common::Size width_ = 0;
			Common::Size height_ = 0;
			DS::Vector<PixelType> pixels_;
			Format format_ = Format::RGBA32;
		};

		Texture(Texture&& moveTexture) noexcept : createInfo_{ } {

			std::swap(createInfo_, moveTexture.createInfo_);
			//std::swap(pixels_, moveTexture.pixels_);

		}

		Texture(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

			//const Common::Size pixelsNumber = createInfo.width_ * createInfo.height_;

			//RGBA32* pixelsMemory = Common::Memory::memory_.Allocate<RGBA32>("texture", pixelsNumber);
			//pixels_ = pixelsMemory;

		}

		//[[nodiscard]]
		//static constexpr Common::Size GetPixelSize(Format format) noexcept {
		//	if (format == Format::RGBA32) {
		//		return sizeof(Color4b);
		//	}
		//	OS::AssertFailMessage("Unsupported type of format.");
		//	return 0;
		//}


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
	Texture<Color4b> CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept;

	//[[nodiscard]]
	//Texture LoadTexture(const std::filesystem::path& texturePath) noexcept {

	//	int texWidth, texHeight, texChannels;
	//	stbi_uc* pixels = stbi_load(texturePath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	//	OS::AssertMessage(pixels != nullptr, { "Error while loading texture %s. %s.", texturePath.string().c_str(), stbi_failure_reason() });

	//	Texture::CreateInfo textureCreateInfo;
	//	{
	//		textureCreateInfo.width_ = texWidth;
	//		textureCreateInfo.height_ = texHeight;
	//		textureCreateInfo.format_ = Texture::Format::RGBA32;
	//	}

	//	Texture texture{ textureCreateInfo };
	//	std::memcpy(texture.GetPixels<Texture::RGBA32>(), pixels, sizeof(Texture::RGBA32) * texture.GetPixelsNumber());

	//	stbi_image_free(pixels);

	//	return texture;

	//}


}
