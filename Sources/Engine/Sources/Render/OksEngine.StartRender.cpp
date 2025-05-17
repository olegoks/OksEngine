#include <Render\auto_OksEngine.StartRender.hpp>

namespace OksEngine {
void StartRender::Update(ECS2::Entity::Id entityId,
                         RenderDriver* renderDriver) {
    
    renderDriver->driver_->StartRender();

};
}