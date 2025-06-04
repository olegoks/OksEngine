
#include <Render/auto_OksEngine.ResizeFrameBuffers.hpp>

namespace OksEngine {




	void ResizeFrameBuffers::Update(
		ECS2::Entity::Id entityId,
		const Window* window,
		FrameBufferResizeEvents* frameBufferResizeEvents) {

		//FrameBufferResizeEvents* events = world->GetComponent<FrameBufferResizeEvents>(entityId);
		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//if (events->HasEvent()) {
		//	UIAL::Window::FrameBufferResizeEvent event = events->GetEvent();
		//	driver->FrameBufferResize({ event.newWidth_, event.newHeight_ });
		//}

	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ResizeFrameBuffers::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<Window>().Include<FrameBufferResizeEvents>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}