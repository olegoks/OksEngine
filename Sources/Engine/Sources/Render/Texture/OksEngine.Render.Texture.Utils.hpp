#pragma once

#include <Render/Texture/auto_OksEngine.Render.Texture.hpp>

#include <stb_image.h>

#define RENDER__TEXTURE__CREATE_TEXTURE_DATA(textureEntityId_, resourcePath_, texture_)\
	[this](																													\
		ECS2::Entity::Id textureEntityId,																					\
		const std::string& resourcePath,																					\
		const aiTexture* texture) {																							\
			Common::UInt32 textureWidth = texture->mWidth;																	\
			Common::UInt32 textureHeight = texture->mHeight;																\
			const bool isTextureCompressed = !(textureHeight > 0);															\
			unsigned char* pixels = (unsigned char*)texture->pcData;														\
			if (isTextureCompressed) {																						\
																															\
				int uncompressedWidth, uncompressedHeight, channels;														\
																															\
				unsigned char* uncompressedPixels = stbi_load_from_memory(													\
					pixels,																									\
					textureWidth,																							\
					&uncompressedWidth, &uncompressedHeight, &channels,														\
					STBI_rgb_alpha																							\
				);																											\
				pixels = uncompressedPixels;																				\
				textureWidth = uncompressedWidth;																			\
				textureHeight = uncompressedHeight;																			\
			}																												\
																															\
			CreateComponent<Render::Texture::Info>(textureEntityId, resourcePath);											\
			CreateComponent<Render::Texture::Data>(																			\
				textureEntityId,																							\
				textureWidth, textureHeight,																				\
				std::vector<Geom::Color4b>{																					\
				(Geom::Color4b*)pixels,																						\
				(Geom::Color4b*)pixels + textureWidth * textureHeight});													\
	}(textureEntityId_, resourcePath_, texture_)

#define RENDER__TEXTURE__CREATE_DIFFUSE_MAP(textureResourcePath, AITexture)										\
	[this](																										\
		const std::string& resourcePath,																		\
		const aiTexture* texture) {																				\
			ECS2::Entity::Id diffuseMapEntityId = CreateEntity<RENDER__TEXTURE__TEXTURE>();						\
			CreateComponent<Render::Texture::Tag>(diffuseMapEntityId);											\
			CreateComponent<Render::Texture::Type::DiffuseMap::Tag>(diffuseMapEntityId);						\
			RENDER__TEXTURE__CREATE_TEXTURE_DATA(diffuseMapEntityId, resourcePath, texture);					\
			return diffuseMapEntityId;																			\
	}(textureResourcePath, AITexture)

#define RENDER__TEXTURE__CREATE_NORMAL_MAP(textureResourcePath, AITexture)										\
	[this](																										\
		const std::string& resourcePath,																		\
		const aiTexture* texture) {																				\
			ECS2::Entity::Id diffuseMapEntityId = CreateEntity<RENDER__TEXTURE__TEXTURE>();						\
			CreateComponent<Render::Texture::Tag>(diffuseMapEntityId);											\
			CreateComponent<Render::Texture::Type::NormalMap::Tag>(diffuseMapEntityId);							\
			RENDER__TEXTURE__CREATE_TEXTURE_DATA(diffuseMapEntityId, resourcePath, texture);					\
			return diffuseMapEntityId;																			\
	}(textureResourcePath, AITexture)

#define RENDER__TEXTURE__CREATE_AMBIENT_MAP(textureResourcePath, AITexture)										\
	[this](																										\
		const std::string& resourcePath,																		\
		const aiTexture* texture) {																				\
			ECS2::Entity::Id ambientMapEntityId = CreateEntity<RENDER__TEXTURE__TEXTURE>();						\
			CreateComponent<Render::Texture::Tag>(ambientMapEntityId);											\
			CreateComponent<Render::Texture::Type::AmbientMap::Tag>(ambientMapEntityId);						\
			RENDER__TEXTURE__CREATE_TEXTURE_DATA(ambientMapEntityId, resourcePath, texture);					\
			return ambientMapEntityId;																			\
	}(textureResourcePath, AITexture)

