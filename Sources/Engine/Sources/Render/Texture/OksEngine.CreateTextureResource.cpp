#include <Render/Texture\auto_OksEngine.CreateTextureResource.hpp>

namespace OksEngine {
void CreateTextureResource::Update(ECS2::Entity::Id entity1Id,
                                      const DriverTexture* driverTexture,
                                      ECS2::Entity::Id entity2Id,
                                      RenderDriver* renderDriver) {
    
    auto driver = renderDriver->driver_;
    
    RAL::Driver::Resource::Binding textureBinding
    {
        .stage_ = RAL::Driver::Stage::FragmentShader,
        .binding_ = 0,
        .textureId_ = driverTexture->driverTextureId_
    };
    
    const RAL::Driver::Resource::Id textureRSId = driver->CreateResource(textureBinding);

    CreateComponent<TextureResource>(entity1Id, textureRSId);

};
}