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

namespace ECS2 {

	class ComponentsManager final {
	public:

		class ArchetypeComponents {
		public:
			ArchetypeComponents() { }

			template<class ComponentType>
			void AddComponent() {
				containers_[ComponentType::GetTypeId()] = std::make_shared<Container<ComponentType>>();
			}

			template<class Component>
			Ptr<IContainer> GetContainer() {
				return containers_[Component::GetTypeId()];
			}

			std::map<ComponentTypeId, std::shared_ptr<IContainer>> containers_;

		};

		template<class ...ComponentType>
		bool IsArchetypeAddedYet() {

			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<ComponentType...>();

			return archetypeComponents_.contains(componentsFilter);
		}

		//Archetype logic.
		template<class ...ComponentType>
		void AddArchetype() {

			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<ComponentType...>();

			OS::AssertMessage(!IsArchetypeAddedYet(), "This archetype was added.");

			ArchetypeComponents archetypeComponents;


			auto callback = [&] <class ComponentType>(){
				archetypeComponents.AddComponent<ComponentType>();
			};

			(archetypeComponents.AddComponent<ComponentType>(), ...);

			archetypeComponents_[componentsFilter] = archetypeComponents;


		}

		std::map<ComponentsFilter, ArchetypeComponents, ComponentsFilter::LessComparator> archetypeComponents_;

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			EntityComponents& entityComponents = GetCreateEntityComponents(entityId);
			OS::AssertMessage(
				!entityComponents.IsComponentExist<ComponentType>(),
				"Attempt to add component to entity, but component exists yet.");
			Ptr<Container<ComponentType>> container = GetCreateContainer<ComponentType>();
			OS::AssertMessage(container != nullptr,
				"Error while creating/getting components container.");
			const ComponentIndex index = container->CreateComponent(std::forward<Args>(args)...);
			entityComponents.AddComponent<ComponentType>(index);
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId) noexcept {
			EntityComponents& entityComponents = GetEntityComponents(entityId);
			OS::AssertMessage(
				entityComponents.IsComponentExist<ComponentType>(),
				"Attempt to remove component that doesnt exist.");
			Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
			OS::AssertMessage(
				maybeContainer.has_value(),
				"Attempt to remove component but container doesnt exist.");
			Ptr<Container<ComponentType>> container = maybeContainer.value();
			entityComponents.RemoveComponent<ComponentType>();
			const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
			OS::AssertMessage(
				(*container)[componentIndex] != nullptr,
				"Attempt to delete component that doesnt exist.");
			container->RemoveComponent(componentIndex);
		}

		//template<class ComponentType>
		//[[nodiscard]]
		//ComponentType* GetComponent(Entity::Id entityId) noexcept {
		//	EntityComponents& entityComponents = GetEntityComponents(entityId);
		//	OS::AssertMessage(
		//		entityComponents.IsComponentExist<ComponentType>(),
		//		"Attempt to get component that doesnt exist.");
		//	Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
		//	OS::AssertMessage(
		//		maybeContainer.has_value(),
		//		"Attempt to get component but container doesnt exist.");
		//	Ptr<Container<ComponentType>> container = maybeContainer.value();
		//	const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
		//	OS::AssertMessage(
		//		container->IsComponentExist(componentIndex),
		//		"Attempt to get component that doesnt exist.");
		//	return (*container)[componentIndex];
		//}

		template<class ComponentType>
		ComponentType* GetComponents() {
			Ptr<Container<ComponentType>> container = componentContainer_[ComponentType::GetTypeId()];
			return container->Get();
		}

		void AddDelayedComponents() {
			for (auto& [componentTypeId, container] : componentContainer_) {
				container->ProcessDelayedRequests();
			}
			for (auto& [entityId, entityComponents] : entityComponents_) {
				entityComponents.ProccessDelayedRequests();
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		const ComponentType* GetComponent(Entity::Id entityId) const noexcept {
			return const_cast<ComponentsManager*>(this)->GetComponent<ComponentType>(entityId);
		}

		[[nodiscard]]
		const Entity::Filter& GetEntityFilter(Entity::Id id) const noexcept {
			return GetEntityComponents(id).GetFilter();
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) const noexcept {
			OS::AssertMessage(!entityId.IsInvalid(), "Attempt to check is component exist using invalid entity id.");
			return GetEntityComponents(entityId).IsComponentExist<ComponentType>();
		}

	private:

		template<class ComponentType>
		[[nodiscard]]
		Ptr<Container<ComponentType>> GetCreateContainer() noexcept {
			Maybe<Ptr<Container<ComponentType>>> maybeComponentContainer = GetContainer<ComponentType>();
			if (maybeComponentContainer.has_value() && maybeComponentContainer.value() != nullptr) {
				return maybeComponentContainer.value();
			}
			else {
				return CreateContainer<ComponentType>();
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		Maybe<Ptr<Container<ComponentType>>> GetContainer() noexcept {
			const ComponentTypeId componentTypeId = IComponent<ComponentType>::GetTypeId();
			auto it = componentContainer_.find(componentTypeId);
			if (it != componentContainer_.end()) {
				return std::dynamic_pointer_cast<Container<ComponentType>>(it->second);
			}
			else {
				return {};
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		Maybe<const Ptr<Container<ComponentType>>> GetContainer() const noexcept {
			const ComponentTypeId componentTypeId = IComponent<ComponentType>::GetTypeId();
			auto it = componentContainer_.find(componentTypeId);
			if (it != componentContainer_.end()) {
				return std::dynamic_pointer_cast<Container<ComponentType>>(it->second);
			}
			else {
				return {};
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		Ptr<Container<ComponentType>> CreateContainer() noexcept {
			const ComponentTypeId componentTypeId = ComponentType::GetTypeId();
			auto createdContainerPointer = std::make_shared<Container<ComponentType>>();
			componentContainer_.insert(std::pair{ componentTypeId, createdContainerPointer });
			return createdContainerPointer;
		}


		[[nodiscard]]
		EntityComponents& GetCreateEntityComponents(Entity::Id entityId) noexcept {
			if (IsEntityComponentsExist(entityId)) {
				return GetEntityComponents(entityId);
			}
			else {
				return CreateEntityComponents(entityId);
			}
		}

		[[nodiscard]]
		bool IsEntityComponentsExist(Entity::Id entityId) const noexcept {
			return entityComponents_.find(entityId) != entityComponents_.end();
		}

		[[nodiscard]]
		bool IsEntityHasComponents(Entity::Id entityId) const noexcept {
			auto it = entityComponents_.find(entityId);
			if (it == entityComponents_.end()) {
				return false;
			}
			else {
				const EntityComponents& entityComponents = it->second;
				const Common::Size componentsNumber = entityComponents.GetComponentsNumber();
				return (componentsNumber == 0);
			}
		}


		[[nodiscard]]
		EntityComponents& GetEntityComponents(Entity::Id entityId) noexcept {
			//OS::AssertMessage(IsEntityComponentsExist(entityId), "Attempt to get entity components, but entity doesn't have them.");
			if (!IsEntityComponentsExist(entityId)) {
				CreateEntityComponents(entityId);
			}

			auto it = entityComponents_.find(entityId);
			return it->second;
		}

		[[nodiscard]]
		const EntityComponents& GetEntityComponents(Entity::Id entityId) const noexcept {
			return const_cast<ComponentsManager*>(this)->GetEntityComponents(entityId);
		}

		EntityComponents& CreateEntityComponents(Entity::Id entityId) noexcept {
			OS::AssertMessage(!IsEntityComponentsExist(entityId),
				"Attempt to create entity components structure when it exists yet.");
			entityComponents_.insert({ entityId, EntityComponents{} });
			return GetEntityComponents(entityId);
		}

		std::map<Entity::Id, EntityComponents> entityComponents_;

		std::map<ComponentTypeId, std::shared_ptr<IContainer>> componentContainer_;

		//std::map<Archetype, >
	};


}