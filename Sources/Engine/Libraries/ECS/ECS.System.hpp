#pragma once

#include<vector>
#include <Common.Types.hpp>

#include <ECS.Entity.hpp>
#include <ECS.Component.hpp>

namespace ECS {

	class World;

	class System {
	public:
		virtual void StartUpdate() = 0;
		virtual void Update(World* world, Entity::Id entityId) = 0;
		virtual void EndUpdate() = 0;
		virtual Common::TypeId GetTypeId() const noexcept = 0;
	};

}