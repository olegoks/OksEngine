
#include <Systems/OksEngine.Render.System.hpp>
#include <Components/OksEngine.RenderableGeometry.hpp>
//#include <OksEngine.Render.Subsystem.hpp>

namespace OksEngine {
	void RenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId) const {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		if (immutableRenderGeometry == nullptr) return;
		/*context_.GetRenderSubsystem()->RenderModel(
			immutableRenderGeometry->modelObjFileName_,
			immutableRenderGeometry->modelMtlFileName_,
			immutableRenderGeometry->modelTextureFileName_);*/

	}
}