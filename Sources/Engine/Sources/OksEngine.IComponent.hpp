#pragma once 

#include <ECS2.Component.hpp>

namespace OksEngine {

    template<class Component>
    struct IComponent : ECS2::IComponent<Component> {

    };


	template<class Component>
	inline void Edit(Component* component) {

	}

}