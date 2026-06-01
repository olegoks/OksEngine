#pragma once 

#include <ECS2.Component.hpp>

namespace OksEngine {

	template <class Component>
	inline void Edit(std::shared_ptr<ECS2::World>, ECS2::Entity::Id entityId, Component* component) { }

	template <class Component> 
	std::string Serialize(Component* OksEngine__Test2__Test2) { };

}