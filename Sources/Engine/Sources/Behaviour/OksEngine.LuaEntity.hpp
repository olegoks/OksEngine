#pragma once 

#include <OksEngine.Components.hpp>

namespace OksEngine {

	namespace Lua {

		class Entity {
		public:

			Position3D* GetPosition();
			Direction3D* GetDirection();
			Active* GetActive();
			Up3D* GetUp3D();
			Width* GetWidth();
			Height* GetHeight();


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

			void SetWorld(ECS2::World* world);

			void SetId(std::uint64_t id);

		private:
			ECS2::World* world_ = nullptr;
			ECS2::Entity::Id id_ = 0;
		};

	}

	inline void BindLuaEntity(::Lua::Context& context) {

		context.GetGlobalNamespace()
			.beginClass<Lua::Entity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &Lua::Entity::GetPosition)
			.addFunction("GetDirection", &Lua::Entity::GetDirection)
			.addFunction("GetImmutableRenderGeometry", &Lua::Entity::GetImmutableRenderGeometry)
			.addFunction("GetCamera", &Lua::Entity::GetCamera)
			.addFunction("GetActive", &Lua::Entity::GetActive)
			.addFunction("GetUp3D", &Lua::Entity::GetUp3D)
			.addFunction("GetHeight", &Lua::Entity::GetHeight)
			.addFunction("GetWidth", &Lua::Entity::GetWidth)
			//.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
			//.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
			.endClass();
	}

}