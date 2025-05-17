#include <Debug/Render/Atlas\auto_OksEngine.CreateImGuiAtlasTextureResource.hpp>

namespace OksEngine {
void CreateImGuiAtlasTextureResource::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const ImGuiAtlasDriverTexture* imGuiAtlasDriverTexture,
    ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {


    auto driver = renderDriver->driver_;

    RAL::Driver::ResourceSet::Binding textureBinding
    {
        .stage_ = RAL::Driver::Stage::FragmentShader,
        .binding_ = 0,
        .textureId_ = imGuiAtlasDriverTexture->driverTextureId_
    };

    const RAL::Driver::Resource::Id textureResourceId = driver->CreateResource(textureBinding);

    CreateComponent<TextureResource>(entity1Id, textureResourceId);

        
    };
}