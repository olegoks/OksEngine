
#include <OksEngine.Render.System.hpp>

#include <OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	RenderSystem::RenderSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void RenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		if (immutableRenderGeometry == nullptr) return;
		if (immutableRenderGeometry->modelId_ == Common::Limits<Common::Index>::Max()) {
			Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModel(
				immutableRenderGeometry->modelObjFileName_,
				immutableRenderGeometry->modelMtlFileName_,
				immutableRenderGeometry->modelTextureFileName_);
			immutableRenderGeometry->modelId_ = modelIndex;
		}
		GetContext().GetRenderSubsystem()->SetModelMatrix(
			immutableRenderGeometry->modelId_,
			immutableRenderGeometry->modelMatrix_);
	}

	Common::TypeId RenderSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<RenderSystem>().GetId();
	}


}