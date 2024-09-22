
#include <Render/OksEngine.Render.System.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	RenderSystem::RenderSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void RenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		return;
		//auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
		//if (immutableRenderGeometry != nullptr) {
		//	if (immutableRenderGeometry->GetId() == Common::Limits<Common::Index>::Max()) {
		//		if (!immutableRenderGeometry->textures_.empty()) {
		//			Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModel(
		//				immutableRenderGeometry->modelObjFileName_,
		//				immutableRenderGeometry->modelMtlFileName_,
		//				{ immutableRenderGeometry->textures_ });
		//			immutableRenderGeometry->SetId(modelIndex);
		//		}
		//		else {
		//			Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModelTextures(
		//				immutableRenderGeometry->modelObjFileName_,
		//				immutableRenderGeometry->modelMtlFileName_);
		//			immutableRenderGeometry->SetId(modelIndex);
		//		}

		//	}
		//	GetContext().GetRenderSubsystem()->SetModelMatrix(
		//		immutableRenderGeometry->GetId(),
		//		immutableRenderGeometry->GetTransform());
		//}
		//return;
		//auto* skinnedGeometry = world->GetComponent<SkinnedGeometry>(entityId);
		//if (skinnedGeometry != nullptr) {
		//if (skinnedGeometry->GetId() == Common::Limits<Common::Index>::Max()) {
		//	if (skinnedGeometry->textures_.empty()) {
		//		Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderAnimationModel(
		//			{ skinnedGeometry->modelObjFileName_ });
		//		skinnedGeometry->SetId(modelIndex);
		//	}
		//	else {
		//		Common::Index modelIndex = GetContext().GetRenderSubsystem()->RenderModelTextures(
		//			skinnedGeometry->modelObjFileName_,
		//			skinnedGeometry->modelMtlFileName_);
		//		skinnedGeometry->SetId(modelIndex);
		//	}
		//	/*GetContext().GetRenderSubsystem()->SetModelMatrix(
		//		skinnedGeometry->GetId(),
		//		skinnedGeometry->GetTransform());*/

		//}

	//}
	}

	Common::TypeId RenderSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<RenderSystem>().GetId();
	}





	void ResizeFrameBuffers::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		FrameBufferResizeEvents* events = world->GetComponent<FrameBufferResizeEvents>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		if (events->HasEvent()) {
			UIAL::Window::FrameBufferResizeEvent event = events->GetEvent();
			driver->FrameBufferResize({ event.newWidth_, event.newHeight_ });
		}

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ResizeFrameBuffers::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Window>().Include<FrameBufferResizeEvents>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}