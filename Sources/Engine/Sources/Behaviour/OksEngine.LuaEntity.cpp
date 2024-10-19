
#include <Behaviour/OksEngine.LuaEntity.hpp>

namespace OksEngine::Lua {


	Position* Entity::GetPosition() {
		auto position = world_->GetComponent<Position>(id_);
		return position;
	}

	Direction* Entity::GetDirection()
	{
		auto direction = world_->GetComponent<Direction>(id_);
		return direction;
	}

	Active* Entity::GetActive()
	{
		auto active = world_->GetComponent<Active>(id_);
		return active;
	}

	Up* Entity::GetUp()
	{
		auto up = world_->GetComponent<Up>(id_);
		return up;
	}

	Width* Entity::GetWidth()
	{
		auto direction = world_->GetComponent<Width>(id_);
		return direction;
	}


	Height* Entity::GetHeight()
	{
		auto direction = world_->GetComponent<Height>(id_);
		return direction;
	}

	ImmutableRenderGeometry* Entity::GetImmutableRenderGeometry() {
		auto position = world_->GetComponent<ImmutableRenderGeometry>(id_);
		return position;
	}

	Camera* Entity::GetCamera() {
		auto camera = world_->GetComponent<Camera>(id_);
		return camera;
	}

	void Entity::SetWorld(ECS::World* world) {
		world_ = world;
	}

	void Entity::SetId(std::uint64_t id) { id_ = id; }

}