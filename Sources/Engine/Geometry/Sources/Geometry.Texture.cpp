
#include <Geometry.Texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Geometry {
	[[nodiscard]]
	Texture<Color4b> CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept {

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)memory_, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		OS::AssertMessage(pixels != nullptr, { "Error while creating texture. %s.",  stbi_failure_reason() });

		Texture<Color4b>::CreateInfo textureCreateInfo;
		{
			textureCreateInfo.width_ = texWidth;
			textureCreateInfo.height_ = texHeight;
			textureCreateInfo.pixels_ = DS::Vector<Color4b>{ (Color4b*)pixels, (Common::Size)texWidth * texHeight };
			textureCreateInfo.format_ = Texture<Color4b>::Format::RGBA32;

		}

		Texture texture{ textureCreateInfo };
		//std::memcpy(texture.GetPixels<Texture::RGBA32>(), pixels, sizeof(Texture::RGBA32) * texture.GetPixelsNumber());

		stbi_image_free(pixels);

		return texture;

	}
}

namespace Geom = Geometry;