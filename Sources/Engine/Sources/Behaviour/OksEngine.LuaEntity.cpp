
#include <Behaviour/OksEngine.LuaEntity.hpp>

namespace OksEngine::Lua {


	Position3D* Entity::GetPosition() {
		/*auto position = world_->GetComponent<Position3D>(id_);
		return position;*/
		return nullptr;
	}

	Direction3D* Entity::GetDirection()
	{
		//auto direction = world_->GetComponent<Direction3D>(id_);
		//return direction;
		return nullptr;
	}

	Active* Entity::GetActive()
	{
		//auto active = world_->GetComponent<Active>(id_);
		//return active;
		return nullptr;
	}

	Up* Entity::GetUp()
	{
		//auto up = world_->GetComponent<Up>(id_);
		//return up;
		return nullptr;
	}

	Width* Entity::GetWidth()
	{
		//auto direction = world_->GetComponent<Width>(id_);
		//return direction;
		return nullptr;
	}


	Height* Entity::GetHeight()
	{
		//auto direction = world_->GetComponent<Height>(id_);
		//return direction;
		return nullptr;
	}

	ImmutableRenderGeometry* Entity::GetImmutableRenderGeometry() {
		//auto position = world_->GetComponent<ImmutableRenderGeometry>(id_);
		//return position;
	
		return nullptr;
	}

	Camera* Entity::GetCamera() {
		//auto camera = world_->GetComponent<Camera>(id_);
		//return camera;
		return nullptr;
	}

	void Entity::SetWorld(ECS2::World* world) {
		world_ = world;
	}

	void Entity::SetId(std::uint64_t id) { id_ = id; }

}