
#include <Geometry.Texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Geometry {
	[[nodiscard]]
	Texture CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept {

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)memory_, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		OS::AssertMessage(pixels != nullptr, { "Error while creating texture. %s.",  stbi_failure_reason() });

		Texture::CreateInfo textureCreateInfo;
		{
			textureCreateInfo.width_ = texWidth;
			textureCreateInfo.height_ = texHeight;
			textureCreateInfo.pixels_ = DS::Vector<Color4b>{ (Color4b*)pixels, (Common::Size)texWidth * texHeight };

		}		
		stbi_image_free(pixels);
		return Texture{ textureCreateInfo };

	}

	[[nodiscard]]
	Texture CreateTexture(Color4b* pixels, Common::Size width, Common::Size height) noexcept {

		Texture::CreateInfo textureCreateInfo;
		{
			textureCreateInfo.width_ = width;
			textureCreateInfo.height_ = height;
			textureCreateInfo.pixels_ = DS::Vector<Color4b>{ (Color4b*)pixels, (Common::Size)width * height };

		}
		return Texture{ textureCreateInfo };

	}

}

namespace Geom = Geometry;