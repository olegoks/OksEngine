#pragma once 

#include <ECS.Entity.hpp>

namespace ECS {

	class Archetype {
	public:

		Archetype(const Entity::Filter& include) {

		}

		Entity::Filter include_;
	};


}