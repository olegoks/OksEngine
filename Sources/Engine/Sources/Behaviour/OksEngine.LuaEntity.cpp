
#include <Behaviour/OksEngine.LuaEntity.hpp>

namespace OksEngine {


	Position* LuaEntity::GetPosition(lua_State* state) {
		auto position = world_->GetComponent<Position>(id_);
		return position;
	}

	Direction* LuaEntity::GetDirection(lua_State* state)
	{
		auto direction = world_->GetComponent<Direction>(id_);
		return direction;
	}

	Active* LuaEntity::GetActive()
	{
		auto active = world_->GetComponent<Active>(id_);
		return active;
	}

	Up* LuaEntity::GetUp()
	{
		auto up = world_->GetComponent<Up>(id_);
		return up;
	}

	Width* LuaEntity::GetWidth()
	{
		auto direction = world_->GetComponent<Width>(id_);
		return direction;
	}


	Height* LuaEntity::GetHeight()
	{
		auto direction = world_->GetComponent<Height>(id_);
		return direction;
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