#pragma once

#include<vector>
#include <Common.Types.hpp>

#include <ECS.Entity.hpp>
#include <ECS.Component.hpp>

namespace ECS {

	class World;
	//class Accessor::Entity;

	class System {
	public:
		virtual void BeforeUpdate(ECS::World* world) = 0; // Calls once at the begging even if there are noe entities
		virtual void StartUpdate() = 0; // Calls for each entity before Update
		virtual void Update(World* world, Entity::Id firstEntityId) = 0;
		virtual void Update(World* world, Entity::Id firstEntityId, Entity::Id secondEntityId) = 0;
		virtual void EndUpdate() = 0; // Callsfor each entity after Update
		virtual void AfterUpdate(ECS::World* world) = 0; // Calls once after end of updating even if there are no entities.

		struct Filter {
			Entity::Filter first_;
			Entity::Filter second_;
		};

		virtual std::pair<Entity::Filter, Entity::Filter> GetFilter() const noexcept = 0;

		[[nodiscard]]
		virtual std::string GetName() const noexcept = 0;

		virtual Common::TypeId GetTypeId() const noexcept = 0;
		virtual ~System() noexcept = default;
	};


	class System2 {
	public:
		//virtual void BeforeUpdate(ECS::World* world) = 0; // Calls once at the begging even if there are noe entities
		//virtual void StartUpdate() = 0; // Calls for each entity before Update
		virtual void Update(World* world, void* component) = 0;
		//virtual void Update(World* world, Entity::Id firstEntityId, Entity::Id secondEntityId) = 0;
		//virtual void EndUpdate() = 0; // Callsfor each entity after Update
		//virtual void AfterUpdate(ECS::World* world) = 0; // Calls once after end of updating even if there are no entities.

		//struct Filter {
		//	Entity::Filter first_;
		//	Entity::Filter second_;
		//};

		//virtual std::pair<Entity::Filter, Entity::Filter> GetFilter() const noexcept = 0;

		[[nodiscard]]
		virtual std::string GetName() const noexcept = 0;

		virtual Common::TypeId GetTypeId() const noexcept = 0;
		virtual ~System2() noexcept = default;
	};

}