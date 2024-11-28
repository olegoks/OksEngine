
#include <Render/OksEngine.ResizeFrameBuffers.hpp>

#include <UI/OksEngine.UI.KeyboardInput.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <UI/OksEngine.UI.Window.hpp>

namespace OksEngine {




	void ResizeFrameBuffers::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		FrameBufferResizeEvents* events = world->GetComponent<FrameBufferResizeEvents>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		if (events->HasEvent()) {
			UIAL::Window::FrameBufferResizeEvent event = events->GetEvent();
			driver->FrameBufferResize({ event.newWidth_, event.newHeight_ });
		}

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ResizeFrameBuffers::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<Window>().Include<FrameBufferResizeEvents>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

}