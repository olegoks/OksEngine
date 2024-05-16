#pragma once

#include <Lua.hpp>
#include <Luabridge/LuaBridge.h>
#include <ECS.hpp>
#include <Components/OksEngine.Position.hpp>

namespace OksEngine {

	class LuaEntity {
	public:

		Position* GetPosition() {
			auto position = world_->GetComponent<Position>(id_);
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


	struct Behaviour : public ECS::IComponent<Behaviour> {
		std::filesystem::path scriptsPathString_;
		std::string scriptName_;
		std::string objectName_;
		lua_State* state_ = nullptr;
		luabridge::LuaRef updater_;
		luabridge::LuaRef object_;
		LuaEntity entity_;

		Behaviour() = default;
		Behaviour(ECS::World* world,
			ECS::Entity::Id entityId,
			std::filesystem::path scriptsPathString,
			std::string scriptName,
			std::string objectName
		);

		void CallUpdater();
	};

	class BehaviourSystem : public ECS::System {
	private:
	public:

		BehaviourSystem() noexcept {

		}

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) const override {
			Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
			behaviour->CallUpdater();

		}
	};


}