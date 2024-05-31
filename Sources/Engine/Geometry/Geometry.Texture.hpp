#pragma once

//#include "Sources\Engine\Resources\stb_image.h"
//#include <vulkan/vulkan.hpp>
#include <filesystem>

//import OksEngine.Common;
//import OksEngine.OS;
//import OksEngine.Math.Vector;
//import OksEngine.Common.MemoryManager;
//import OksEngine.Common.Memory.BlockAllocator;

namespace Geometry {

	class Texture {
	public:

		//enum class Format : Common::UInt64 {
		//	RGBA32
		//};

		//using RGBA32 = Math::Vector3b;

		//struct CreateInfo {
		//	Common::Size width_ = 0;
		//	Common::Size height_ = 0;
		//	Format format_ = Format::RGBA32;
		//};

		//using Allocator = Common::Memory::StdCompatible::DynamicPoolAllocator;

		//Texture(Texture&& moveTexture) noexcept : createInfo_{ }, pixels_{ nullptr } {

		//	std::swap(createInfo_, moveTexture.createInfo_);
		//	std::swap(pixels_, moveTexture.pixels_);

		//}

		//Texture(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {

		//	const Common::Size pixelsNumber = createInfo.width_ * createInfo.height_;
		//	
		//	RGBA32* pixelsMemory = Common::Memory::memory_.Allocate<RGBA32>("texture", pixelsNumber);
		//	pixels_ = pixelsMemory;

		//}

		//[[nodiscard]]
		//static constexpr Common::Size GetPixelSize(Format format) noexcept {
		//	if (format == Format::RGBA32) {
		//		return sizeof(RGBA32);
		//	}
		//	OS::AssertFailMessage("Unsupported type of format.");
		//	return 0;
		//}

		//[[nodiscard]]
		//RGBA32* operator[](Common::Size column) noexcept {
		//	OS::NotImplemented();
		//	return nullptr;
		//}

		//[[nodiscard]]
		//Common::Size GetWidth() const noexcept { return createInfo_.width_; }

		//[[nodiscard]]
		//Common::Size GetHeight() const noexcept { return createInfo_.height_; }

		//[[nodiscard]]
		//Common::Size GetPixelsNumber() const noexcept { return GetWidth() * GetHeight(); }

		//template<class PixelType>
		//PixelType* GetPixels() noexcept {
		//	return reinterpret_cast<PixelType*>(pixels_);
		//}

		//~Texture() noexcept {
		//	if (pixels_ != nullptr) {
		//		OS::AssertMessage(GetPixelsNumber() != 0, "Invalide texture state.");
		//		Common::Memory::memory_.Deallocate(pixels_);
		//		pixels_ = nullptr;
		//	} else {
		//		OS::AssertMessage(GetPixelsNumber() == 0, "Invalide texture state.");
		//	}
		//	
		//}

	private:
		/*CreateInfo createInfo_;
		RGBA32* pixels_ = nullptr;*/
	};

	//[[nodiscard]]
	//Texture CreateTexture(const Common::Byte* memory_, Common::Size size) noexcept {

	//	int texWidth, texHeight, texChannels;
	//	stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)memory_, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	//	OS::AssertMessage(pixels != nullptr, { "Error while creating texture. %s.",  stbi_failure_reason() });

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
