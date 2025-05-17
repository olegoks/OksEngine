#include <Render/Model/Transform\auto_OksEngine.CreateTransform2DResource.hpp>

namespace OksEngine {
void CreateTransform2DResource::Update(ECS2::Entity::Id entity1Id,
                                          const DriverTransform2D* transform2D,
                                          ECS2::Entity::Id entity2Id,
                                          RenderDriver* renderDriver) {
    
    auto driver = renderDriver->driver_;

    const RAL::Driver::Resource::CI2 rci{
        .stage_ = RAL::Driver::Stage::VertexShader,
        .binding_ = 0,
        .ubid_ = transform2D->id_,
        .offset_ = 0,
        .size_ = driver->GetUBSizeInBytes(transform2D->id_)
    };

    const RAL::Driver::Resource::Id rid = driver->CreateResource(rci);

    CreateComponent<Transform2DResource>(entity1Id, rid);

};
}