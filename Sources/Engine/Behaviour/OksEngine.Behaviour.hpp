#pragma once

#include <filesystem>

#include <Lua.hpp>
#include <luabridge3/LuaBridge/LuaBridge.h>
#include <ECS.hpp>
#include <Common/OksEngine.Position.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.ECS.System.hpp>
#include <OksEngine.ECS.Component.hpp>
#include <Lua.Context.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>
#include <Physics/OksEngine.DynamicRigidBodyBox.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCapsule.hpp>
#include <Render/OksEngine.Camera.hpp>

namespace OksEngine {

	class LuaEntity {
	public:

		Position* GetPosition() {
			auto position = world_->GetComponent<Position>(id_);
			return position;
		}

		ImmutableRenderGeometry* GetImmutableRenderGeometry() {
			auto position = world_->GetComponent<ImmutableRenderGeometry>(id_);
			return position;
		}

		DynamicRigidBodyBox* GetRigidBodyBox() {
			auto rigidBody = world_->GetComponent<DynamicRigidBodyBox>(id_);
			return rigidBody;
		}

		DynamicRigidBodyCapsule* GetRigidBodyCapsule() {
			auto rigidBody = world_->GetComponent<DynamicRigidBodyCapsule>(id_);
			return rigidBody;
		}

		Camera* GetCamera() {
			auto camera = world_->GetComponent<Camera>(id_);
			return camera;
		}

		void SetWorld(ECS::World* world) {
			world_ = world;
		}

		void SetId(std::uint64_t id) { id_ = id; }

	private:
		ECS::World* world_ = nullptr;
		ECS::Entity::Id id_ = 0;
	};


	struct Behaviour : public ECSComponent<Behaviour> {
		std::string scriptName_;
		std::string objectName_;
		lua_State* state_ = nullptr;
		luabridge::LuaRef updater_;
		luabridge::LuaRef inputProcessor_;
		luabridge::LuaRef object_;
		LuaEntity entity_;
		Lua::Context  context_;
		std::chrono::high_resolution_clock::time_point previousUpdateTimePoint_;

		Behaviour() = default;
		Behaviour(Context* context,
			ECS::Entity::Id entityId,
			std::string scriptName,
			std::string objectName
		);

		void CallUpdater(Common::Size ms);
		void CallInputProcessor(const char* inputKey, const char* inputEvent, double offsetX, double offsetY);
	};

	template<>
	inline void Edit<Behaviour>(Behaviour* behaviour) {
		ImGui::TextDisabled("Script name: %s", behaviour->scriptName_.c_str());
		ImGui::TextDisabled("Object name: %s", behaviour->objectName_.c_str());
	}

	class BehaviourSystem : public ECSSystem {
	private:
	public:

		BehaviourSystem(Context& context) noexcept : ECSSystem{ context } {

		}

		virtual void StartUpdate() override { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
			if (behaviour == nullptr) return;
			auto now = std::chrono::high_resolution_clock::now();
			auto delta = now - behaviour->previousUpdateTimePoint_;
			behaviour->CallUpdater(std::chrono::duration_cast<std::chrono::milliseconds>(delta).count());
			behaviour->previousUpdateTimePoint_ = now;
		}

		virtual void EndUpdate() override { }

		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<Behaviour>();
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<BehaviourSystem>().GetId();
		}

	private:
		
	};


}