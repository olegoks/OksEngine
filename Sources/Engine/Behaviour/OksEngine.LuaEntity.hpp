#pragma once 

#include <OksEngine.Components.hpp>

namespace OksEngine {


	class LuaEntity {
	public:

		Position* GetPosition();

		ImmutableRenderGeometry* GetImmutableRenderGeometry();

		//DynamicRigidBodyBox* GetRigidBodyBox() {
		//	auto rigidBody = world_->GetComponent<DynamicRigidBodyBox>(id_);
		//	return rigidBody;
		//}

		//DynamicRigidBodyCapsule* GetRigidBodyCapsule() {
		//	auto rigidBody = world_->GetComponent<DynamicRigidBodyCapsule>(id_);
		//	return rigidBody;
		//}

		Camera* GetCamera();

		void SetWorld(ECS::World* world);

		void SetId(std::uint64_t id);

	private:
		ECS::World* world_ = nullptr;
		ECS::Entity::Id id_ = 0;
	};


}