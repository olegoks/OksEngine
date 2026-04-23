#pragma once

#include <ECS2.World.hpp>

#include<magic_enum/magic_enum.hpp>

namespace OksEngine {
	void EditEntity(std::shared_ptr<ECS2::World> world_, ECS2::Entity::Id entity1id);

}