
#include <OksEngine.Render.System.hpp>

namespace OksEngine {


	RenderSystem::RenderSystem(Context& context) noexcept :
		context_{ context } { }

	void RenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId) const {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		if (immutableRenderGeometry == nullptr) return;
		//context_.GetRenderSubsystem()->

	}

	Common::TypeId RenderSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<RenderSystem>().GetId();
	}


}