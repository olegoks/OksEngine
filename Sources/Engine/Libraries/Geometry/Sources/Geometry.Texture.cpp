
#include <Geometry.Texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Geometry {
	[[nodiscard]]
	std::shared_ptr<Texture> CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept {

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
		auto texture = std::make_shared<Texture>(std::move(textureCreateInfo));
		return texture;

	}
}

namespace Geom = Geometry;