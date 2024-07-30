
#include <OksEngine.Render.System.hpp>

#include <OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	RenderSystem::RenderSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void RenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		if (immutableRenderGeometry != nullptr) {
			if (immutableRenderGeometry->GetId() == Common::Limits<Common::Index>::Max()) {
				if (!immutableRenderGeometry->textures_.empty()) {
					Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModel(
						immutableRenderGeometry->modelObjFileName_,
						immutableRenderGeometry->modelMtlFileName_,
						{ immutableRenderGeometry->textures_ });
					immutableRenderGeometry->SetId(modelIndex);
				}
				else {
					Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModelTextures(
						immutableRenderGeometry->modelObjFileName_,
						immutableRenderGeometry->modelMtlFileName_);
					immutableRenderGeometry->SetId(modelIndex);
				}

			}
			GetContext().GetRenderSubsystem()->SetModelMatrix(
				immutableRenderGeometry->GetId(),
				immutableRenderGeometry->GetTransform());
		}
		auto* skinnedGeometry = world->GetComponent<SkinnedGeometry>(entityId);
		if (skinnedGeometry != nullptr) {
		if (skinnedGeometry->GetId() == Common::Limits<Common::Index>::Max()) {
			if (skinnedGeometry->textures_.empty()) {
				Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderAnimationModel(
					skinnedGeometry->modelObjFileName_,
					skinnedGeometry->modelMtlFileName_,
					{ skinnedGeometry->textures_ });
				skinnedGeometry->SetId(modelIndex);
			}
			else {
				Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModelTextures(
					skinnedGeometry->modelObjFileName_,
					skinnedGeometry->modelMtlFileName_);
				skinnedGeometry->SetId(modelIndex);
			}

		}
		GetContext().GetRenderSubsystem()->SetModelMatrix(
			skinnedGeometry->GetId(),
			skinnedGeometry->GetTransform());
	}
	}

	Common::TypeId RenderSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<RenderSystem>().GetId();
	}



	CameraSystem::CameraSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void CameraSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* camera = world->GetComponent<Camera>(entityId);
		auto* position = world->GetComponent<Position>(entityId);
		if (camera == nullptr) return;
		if (camera->isActive_) {
			GetContext().GetRenderSubsystem()->SetCamera(camera->position_, camera->direction_, camera->up_);
		}
	}

	Common::TypeId CameraSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<CameraSystem>().GetId();
	}



}