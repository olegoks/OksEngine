
#include <Behaviour/OksEngine.LuaEntity.hpp>


namespace OksEngine {


	void BindLuaEntity(::Lua::Context& context) {

		context.GetGlobalNamespace()
			.beginClass<Entity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetWorldPosition3D", &Entity::GetWorldPosition3D)
			.addFunction("GetDirection3D", &Entity::GetDirection3D)
			.addFunction("GetImmutableRenderGeometry", &Entity::GetImmutableRenderGeometry)
			.addFunction("GetSetVelocityRequests", &Entity::GetSetVelocityRequests)
			.addFunction("GetRigidBodyEntityId", &Entity::GetRigidBodyEntityId)
			.addFunction("GetCamera", &Entity::GetCamera)
			.addFunction("GetActive", &Entity::GetActive)
			.addFunction("GetUp3D", &Entity::GetUp3D)
			.addFunction("GetHeight", &Entity::GetHeight)
			.addFunction("GetWidth", &Entity::GetWidth)
			.addFunction("SetWorld", &Entity::SetWorld)
			.addFunction("SetId", &Entity::SetId)
			.addFunction("GetId", &Entity::GetId)
			//.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
			//.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
			.endClass();
	}

	WorldPosition3D* Entity::GetWorldPosition3D() {
		auto position = world_->GetComponent<WorldPosition3D>(id_);
		return position;
	}

	Direction3D* Entity::GetDirection3D()
	{
		auto direction = world_->GetComponent<Direction3D>(id_);
		return direction;
	}

	Active* Entity::GetActive()
	{
		auto active = world_->GetComponent<Active>(id_);
		return active;
	}

	Up3D* Entity::GetUp3D()
	{
		auto up = world_->GetComponent<Up3D>(id_);
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

	Physics::SetVelocityRequests* Entity::GetSetVelocityRequests() {
		auto direction = world_->GetComponent<Physics::SetVelocityRequests>(id_);
		return direction;
	}

	Physics::RigidBodyEntityId* Entity::GetRigidBodyEntityId() {
		auto direction = world_->GetComponent<Physics::RigidBodyEntityId>(id_);
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

	void Entity::SetWorld(ECS2::World* world) {
		world_ = world;
	}

	void Entity::SetId(std::uint64_t id) { 
		id_ = id;
	}

	std::uint64_t Entity::GetId() {
		return	id_;
	}
};
