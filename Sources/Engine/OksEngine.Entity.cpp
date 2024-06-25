
#include <OksEngine.Entity.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.RenderableGeometry.hpp>

namespace OksEngine {


	void Entity::AddDebugRenderableGeometry() {
		world_->CreateComponent<DebugRenderableGeometry>(GetId());
	}

	void Entity::AddBehaviour(
		const char* scriptName,
		const char* objectName) {
		world_->CreateComponent<Behaviour>(
			GetId(),
			context_,
			GetId(),
			scriptName,
			objectName);
	}

	void Entity::AddPosition(int x, int y, int z) {
		world_->CreateComponent<Position>(GetId(), x, y, z);
	}

}