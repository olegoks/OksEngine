#pragma once

#include <regex>

#include <auto_OksEngine.ECS.Component.Utils.hpp>
#include <auto_ECSGenerator3.Project.hpp>

#define COMPONENT_TYPE_COMMA(type) type,
#define COMPONENT_TYPE(type) type
#define COMPONENT_TYPES_LIST() FOR_EACH_COMPONENT(COMPONENT_TYPE_COMMA, COMPONENT_TYPE)


ECS2::ComponentTypeId GetComponentTypeIdByFullName(const char* componentName);
std::string GetFullNameByComponentTypeId(ECS2::ComponentTypeId componentTypeId);

#define BIND_COMPONENT(type)\
	if(type::IsBindable()) {\
		Bind<type>(state);\
	}

void BindComponents(lua_State* state) {
	FOR_EACH_COMPONENT(BIND_COMPONENT, BIND_COMPONENT);
}


#define GET_COMPONENT_NAME(componentType)\
	std::regex_replace(std::string{ #componentType }, std::regex("::"), "_")

#define BIND_GET_FUNCTION(componentType)\
	.addFunction(("Get" + GET_COMPONENT_NAME(componentType)).c_str(), \
	[](ECS2::World* world, ECS2::Entity::Id::ValueType entityId) {\
		auto* componentPtr = world->GetComponent<componentType>(entityId);\
		return componentPtr;\
	})

void BindWorld(lua_State* state) {
	luabridge::Namespace namespaceGlobal = luabridge::getGlobalNamespace(state);
	namespaceGlobal.beginClass<ECS2::World>("ECSWorld")
		.addConstructor<void(*)()>()
		.addFunction("IsComponentExist", [](ECS2::World* world, ECS2::Entity::Id::ValueType entityId, std::string componentName) {
		const ECS2::ComponentTypeId componentTypeId = GetComponentTypeIdByFullName(componentName.c_str());
		return world->IsComponentExist(entityId, componentTypeId);
			})
		.addFunction("RemoveComponent", [](ECS2::World* world, ECS2::Entity::Id::ValueType entityId, std::string componentName) {
		const ECS2::ComponentTypeId componentTypeId = GetComponentTypeIdByFullName(componentName.c_str());
		return world->RemoveComponent(entityId, componentTypeId);
			})
		FOR_EACH_COMPONENT(BIND_GET_FUNCTION, BIND_GET_FUNCTION)
		.endClass();
}

template<class ComponentType>
bool IsBindable() {
	return ComponentType::IsBindable();
}

template<class ComponentType>
bool IsSerializable() {
	return ComponentType::IsSerializable();
}

