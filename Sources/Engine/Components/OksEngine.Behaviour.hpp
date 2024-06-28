#pragma once

#include <filesystem>

#include <Lua.hpp>
#include <luabridge3/LuaBridge/LuaBridge.h>
#include <ECS.hpp>
#include <Components/OksEngine.Position.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.ECS.Component.hpp>
#include <Lua.Context.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>


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
		luabridge::LuaRef object_;
		LuaEntity entity_;
		Lua::Context  context_;

		Behaviour() = default;
		Behaviour(Context* context,
			ECS::Entity::Id entityId,
			std::string scriptName,
			std::string objectName
		);

		void CallUpdater(Common::Size ms);
	};

	class BehaviourSystem : public ECS::System {
	private:
	public:

		BehaviourSystem() noexcept {

		}

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
			if (behaviour == nullptr) return;

			auto now = std::chrono::high_resolution_clock::now();
			auto delta = now - previousUpdateTimePoint_;
			behaviour->CallUpdater(std::chrono::duration_cast<std::chrono::milliseconds>(delta).count());
			previousUpdateTimePoint_ = now;
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<BehaviourSystem>().GetId();
		}
	private:
		std::chrono::high_resolution_clock::time_point previousUpdateTimePoint_;
	};


}