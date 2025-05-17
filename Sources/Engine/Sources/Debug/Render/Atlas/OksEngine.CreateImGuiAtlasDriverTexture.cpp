#include <Debug/Render/Atlas\auto_OksEngine.CreateImGuiAtlasDriverTexture.hpp>

namespace OksEngine {
void CreateImGuiAtlasDriverTexture::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const ImGuiAtlasTexture* imGuiAtlasTexture, ECS2::Entity::Id entity2Id,
    RenderDriver* renderDriver) {

	auto driver = renderDriver->driver_;

	std::vector<RAL::Color4b> texturePixels{ 
		imGuiAtlasTexture->pixels_.GetData(),
		imGuiAtlasTexture->pixels_.GetData() + imGuiAtlasTexture->pixels_.GetSize() };

	RAL::Texture::CreateInfo textureCreateInfo{
		.name_ = "",
		.pixels_ = texturePixels,
		.size_ = { 
			imGuiAtlasTexture->width_,
			imGuiAtlasTexture->height_ }
	};
	RAL::Texture::Id textureId = driver->CreateDiffuseMap(textureCreateInfo);

	CreateComponent<ImGuiAtlasDriverTexture>(entity1Id, textureId);
        
    };
}