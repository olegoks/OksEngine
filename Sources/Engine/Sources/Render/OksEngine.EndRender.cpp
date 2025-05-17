#include <Render\auto_OksEngine.EndRender.hpp>

namespace OksEngine {
void EndRender::Update(ECS2::Entity::Id entityId, RenderDriver* renderDriver) {

	renderDriver->driver_->EndRender();

};
}