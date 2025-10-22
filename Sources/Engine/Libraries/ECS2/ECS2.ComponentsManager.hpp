#pragma once

#include <vector>
#include <map>
#include <memory>

#include <ECS2.Common.hpp>
#include <ECS2.Entity.hpp>
#include <ECS2.Component.hpp>
#include <ECS2.ComponentsFilter.hpp>
#include <ECS2.Archetype.hpp>
#include <ECS2.Container.hpp>
#include <ECS2.EntityComponents.hpp>
#include <ECS2.ArchetypeComponents.hpp>

namespace ECS2 {

	class ComponentsManager final {
	public:

//
//		template<class ...ComponentType>
//		bool IsArchetypeAdded() {
//
//			ComponentsFilter componentsFilter;
//			componentsFilter.SetBits<ComponentType...>();
//
//			return archetypeEntitiesComponents_.contains(componentsFilter);
//		}
//
//		template<class ...Components>
//		void ForEachArchetypeEntity(
//			const ComponentsFilter& exclude,
//			std::function<void(Components*...)>&& processEntity) {
//
//			ComponentsFilter include;
//			include.SetBits<Components...>();
//			auto archetypeComponentIt = archetypeEntitiesComponents_.find(include);
//			if (archetypeComponentIt != archetypeEntitiesComponents_.end()) {
//				auto archetypeComponents = (*archetypeComponentIt).second;
//				archetypeComponents->ForEachEntity<Components...>(std::forward<std::function<void(Components*...)>&&>(processEntity));
//			}
//		}
//
//		//Archetype logic.
//		template<class ...ComponentType>
//		void AddArchetype() {
//
//			ComponentsFilter componentsFilter;
//			componentsFilter.SetBits<ComponentType...>();
//#pragma region Assert
//			ASSERT_FMSG(!IsArchetypeAdded(), "This archetype was added.");
//#pragma endregion
//			archetypeEntitiesComponents_[componentsFilter] = std::make_shared<ArchetypeComponents<ComponentType...>>();
//
//
//
//		}
//
//		std::map<
//			ComponentsFilter,
//			std::shared_ptr<IArchetypeComponents>,
//			ComponentsFilter::LessComparator> archetypeEntitiesComponents_;
//
//		template<class ComponentType, class ...Args>
//		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
//			EntityInfo& entityComponents = GetCreateEntityComponents(entityId);
//#pragma region Assert
//			ASSERT_FMSG(
//				!entityComponents.IsComponentExist<ComponentType>(),
//				"Attempt to add component to entity, but component exists yet.");
//#pragma endregion
//			Ptr<Container<ComponentType>> container = GetCreateContainer<ComponentType>();
//#pragma region Assert
//			ASSERT_FMSG(container != nullptr,
//				"Error while creating/getting components container.");
//#pragma endregion
//			const ComponentIndex index = container->CreateComponent(std::forward<Args>(args)...);
//			entityComponents.AddComponent<ComponentType>(index);
//		}
//
//		template<class ComponentType>
//		void RemoveComponent(Entity::Id entityId) noexcept {
//			EntityInfo& entityComponents = GetEntityComponents(entityId);
//#pragma region Assert
//			ASSERT_FMSG(
//				entityComponents.IsComponentExist<ComponentType>(),
//				"Attempt to remove component that doesnt exist.");
//#pragma endregion
//			Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
//#pragma region Assert
//			ASSERT_FMSG(
//				maybeContainer.has_value(),
//				"Attempt to remove component but container doesnt exist.");
//#pragma endregion
//			Ptr<Container<ComponentType>> container = maybeContainer.value();
//			entityComponents.RemoveComponent<ComponentType>();
//			const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
//#pragma region Assert
//			ASSERT_FMSG(
//				(*container)[componentIndex] != nullptr,
//				"Attempt to delete component that doesnt exist.");
//#pragma endregion
//			container->RemoveComponent(componentIndex);
//		}
//
//		template<class ...Components>
//		[[nodiscard]]
//		std::shared_ptr<ArchetypeComponents<Components...>> GetArchetypeComponents() {
//			ComponentsFilter componentsFilter;
//			componentsFilter.SetBits<Components...>();
//			std::shared_ptr<IArchetypeComponents> archetypeComponents = GetArchetypeComponents(componentsFilter);
//			return dynamic_cast<std::shared_ptr<ArchetypeComponents<Components...>>>(archetypeComponents);
//		}
//
//		[[nodiscard]]
//		std::shared_ptr<IArchetypeComponents> GetArchetypeComponents(const ComponentsFilter& componentsFilter) {
//			return archetypeEntitiesComponents_[componentsFilter];
//		}


	//	//template<class ComponentType> 
	//	//[[nodiscard]]
	//	//ComponentType* GetComponent(Entity::Id entityId) noexcept {
	//	//	EntityComponents& entityComponents = GetEntityComponents(entityId);
	//	//	ASSERT_FMSG(
	//	//		entityComponents.IsComponentExist<ComponentType>(),
	//	//		"Attempt to get component that doesnt exist.");
	//	//	Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
	//	//	ASSERT_FMSG(
	//	//		maybeContainer.has_value(),
	//	//		"Attempt to get component but container doesnt exist.");
	//	//	Ptr<Container<ComponentType>> container = maybeContainer.value();
	//	//	const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
	//	//	ASSERT_FMSG(
	//	//		container->IsComponentExist(componentIndex),
	//	//		"Attempt to get component that doesnt exist.");
	//	//	return (*container)[componentIndex];
	//	//}

	//	template<class ComponentType>
	//	ComponentType* GetComponents() {
	//		Ptr<Container<ComponentType>> container = componentContainer_[ComponentType::GetTypeId()];
	//		return container->Get();
	//	}

	//	void AddDelayedComponents() {
	//		/*for (auto& [componentTypeId, container] : componentContainer_) {
	//			container->ProcessDelayedRequests();
	//		}
	//		for (auto& [entityId, entityComponents] : entityComponents_) {
	//			entityComponents.ProccessDelayedRequests();
	//		}*/
	//	}

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	ComponentType* GetComponent(Entity::Id entityId) noexcept {
	//		return nullptr;//return const_cast<ComponentsManager*>(this)->GetComponent<ComponentType>(entityId);
	//	}

	//	//[[nodiscard]]
	//	//const Entity::Filter& GetEntityFilter(Entity::Id id) const noexcept {
	//	//	return GetEntityComponents(id).GetFilter();
	//	//}

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	bool IsComponentExist(Entity::Id entityId) const noexcept {
	//		ASSERT_FMSG(!entityId.IsInvalid(), "Attempt to check is component exist using invalid entity id.");
	//		return GetEntityComponents(entityId).IsComponentExist<ComponentType>();
	//	}

	//private:

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	Ptr<Container<ComponentType>> GetCreateContainer() noexcept {
	//		Maybe<Ptr<Container<ComponentType>>> maybeComponentContainer = GetContainer<ComponentType>();
	//		if (maybeComponentContainer.has_value() && maybeComponentContainer.value() != nullptr) {
	//			return maybeComponentContainer.value();
	//		}
	//		else {
	//			return CreateContainer<ComponentType>();
	//		}
	//	}

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	Maybe<Ptr<Container<ComponentType>>> GetContainer() noexcept {
	//		const ComponentTypeId componentTypeId = IComponent<ComponentType>::GetTypeId();
	//		auto it = componentContainer_.find(componentTypeId);
	//		if (it != componentContainer_.end()) {
	//			return std::dynamic_pointer_cast<Container<ComponentType>>(it->second);
	//		}
	//		else {
	//			return {};
	//		}
	//	}

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	Maybe<const Ptr<Container<ComponentType>>> GetContainer() const noexcept {
	//		const ComponentTypeId componentTypeId = IComponent<ComponentType>::GetTypeId();
	//		auto it = componentContainer_.find(componentTypeId);
	//		if (it != componentContainer_.end()) {
	//			return std::dynamic_pointer_cast<Container<ComponentType>>(it->second);
	//		}
	//		else {
	//			return {};
	//		}
	//	}

	//	template<class ComponentType>
	//	[[nodiscard]]
	//	Ptr<Container<ComponentType>> CreateContainer() noexcept {
	//		const ComponentTypeId componentTypeId = ComponentType::GetTypeId();
	//		auto createdContainerPointer = std::make_shared<Container<ComponentType>>();
	//		componentContainer_.insert(std::pair{ componentTypeId, createdContainerPointer });
	//		return createdContainerPointer;
	//	}


	//	[[nodiscard]]
	//	EntityInfo& GetCreateEntityComponents(Entity::Id entityId) noexcept {
	//		if (IsEntityComponentsExist(entityId)) {
	//			return GetEntityComponents(entityId);
	//		}
	//		else {
	//			return CreateEntityComponents(entityId);
	//		}
	//	}

	//	[[nodiscard]]
	//	bool IsEntityComponentsExist(Entity::Id entityId) const noexcept {
	//		return entityComponents_.find(entityId) != entityComponents_.end();
	//	}

		//[[nodiscard]]
		//bool IsEntityHasComponents(Entity::Id entityId) const noexcept {
		//	auto it = entityComponents_.find(entityId);
		//	if (it == entityComponents_.end()) {
		//		return false;
		//	}
		//	else {
		//		const EntityInfo& entityComponents = it->second;
		//		const Common::Size componentsNumber = entityComponents.GetComponentsNumber();
		//		return (componentsNumber == 0);
		//	}
		//}


		//[[nodiscard]]
		//EntityInfo& GetEntityComponents(Entity::Id entityId) noexcept {
		//	//ASSERT_FMSG(IsEntityComponentsExist(entityId), "Attempt to get entity components, but entity doesn't have them.");
		//	if (!IsEntityComponentsExist(entityId)) {
		//		CreateEntityComponents(entityId);
		//	}

		//	auto it = entityComponents_.find(entityId);
		//	return it->second;
		//}

		//[[nodiscard]]
		//const EntityInfo& GetEntityComponents(Entity::Id entityId) const noexcept {
		//	return const_cast<ComponentsManager*>(this)->GetEntityComponents(entityId);
		//}

		//EntityInfo& CreateEntityComponents(Entity::Id entityId) noexcept {
		//	ASSERT_FMSG(!IsEntityComponentsExist(entityId),
		//		"Attempt to create entity components structure when it exists yet.");
		//	entityComponents_.insert({ entityId, EntityInfo{} });
		//	return GetEntityComponents(entityId);
		//}

		std::unordered_map<Entity::Id, ComponentsFilter, Entity::Id::Hash> entityComponents_;

		//std::map<ComponentTypeId, std::shared_ptr<IContainer>> componentContainer_;



		//std::map<Archetype, >
	};


}