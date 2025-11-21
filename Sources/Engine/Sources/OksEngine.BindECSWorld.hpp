#pragma once

#include <ECS2.World.hpp>
#include <Lua.Context.hpp>
#include <auto_OksEngine.Utils.hpp>

namespace OksEngine {

	void BindECSWorld(::Lua::Context& context) {

		//TODO: Generate this code for all components.
		context.GetGlobalNamespace()
			.beginClass<ECS2::World>("ECSWorld")
			.addConstructor<void(*)()>()
			.addFunction("IsComponentExist", [](ECS2::World* world, ECS2::Entity::Id::ValueType entityId, std::string componentName){

				const ECS2::ComponentTypeId componentTypeId = GetComponentTypeIdByName(componentName);
				return world->IsComponentExist(entityId, componentTypeId);

			})
			.addFunction("GetSetVelocityRequests", [](ECS2::World* world, ECS2::Entity::Id::ValueType entityId) {
				auto* componentPtr = world->GetComponent<Physics::SetVelocityRequests>(entityId);
				return componentPtr;

				})
			.endClass();

	}


}