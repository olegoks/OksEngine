#pragma once 

#include <ECS2.World.hpp>

#include <Lua.Context.hpp>

namespace OksEngine {

		class Entity {
		public:

			class WorldPosition3D* GetWorldPosition3D();
			class Direction3D* GetDirection3D();
			class Active* GetActive();
			class Up3D* GetUp3D();
			class Width* GetWidth();
			class Height* GetHeight();


			class ImmutableRenderGeometry* GetImmutableRenderGeometry();

			//DynamicRigidBodyBox* GetRigidBodyBox() {
			//	auto rigidBody = world_->GetComponent<DynamicRigidBodyBox>(id_);
			//	return rigidBody;
			//}

			//DynamicRigidBodyCapsule* GetRigidBodyCapsule() {
			//	auto rigidBody = world_->GetComponent<DynamicRigidBodyCapsule>(id_);
			//	return rigidBody;
			//}

			class Camera* GetCamera();

			void SetWorld(ECS2::World* world);

			void SetId(std::uint64_t id);
			std::uint64_t GetId();

		private:
			ECS2::World* world_ = nullptr;
			ECS2::Entity::Id id_ = ECS2::Entity::Id::invalid_;
		};


	void BindLuaEntity(::Lua::Context& context);

}