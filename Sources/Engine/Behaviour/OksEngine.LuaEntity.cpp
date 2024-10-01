
#include <Behaviour/OksEngine.LuaEntity.hpp>

namespace OksEngine {


	Position* LuaEntity::GetPosition() {
		auto position = world_->GetComponent<Position>(id_);
		return position;
	}

	ImmutableRenderGeometry* LuaEntity::GetImmutableRenderGeometry() {
		auto position = world_->GetComponent<ImmutableRenderGeometry>(id_);
		return position;
	}

	Camera* LuaEntity::GetCamera() {
		auto camera = world_->GetComponent<Camera>(id_);
		return camera;
	}

	void LuaEntity::SetWorld(ECS::World* world) {
		world_ = world;
	}

	void LuaEntity::SetId(std::uint64_t id) { id_ = id; }

}