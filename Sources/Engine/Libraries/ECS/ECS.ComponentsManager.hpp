#pragma once

#include <vector>
#include <map>
#include <memory>

#include <ECS.Common.hpp>
#include <ECS.Entity.hpp>
#include <ECS.Component.hpp>

namespace ECS {

	class ComponentsManager final {
	public:
		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			EntityComponents& entityComponents = GetCreateEntityComponents(entityId);
			OS::AssertMessage(
				!entityComponents.IsComponentExist<ComponentType>(),
				"Attempt to add component to entity, but component exists yet.");
			Ptr<Container<ComponentType>> container = GetCreateContainer<ComponentType>();
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
			const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
			OS::AssertMessage(
				(*container)[componentIndex] != nullptr,
				"Attempt to delete component that doesnt exist.");
			container->RemoveComponent(componentIndex);
		}

		template<class ComponentType>
		[[nodiscard]]
		ComponentType* GetComponent(Entity::Id entityId) noexcept {
			EntityComponents& entityComponents = GetEntityComponents(entityId);
			OS::AssertMessage(
				entityComponents.IsComponentExist<ComponentType>(),
				"Attempt to get component that doesnt exist.");
			Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
			OS::AssertMessage(
				maybeContainer.has_value(),
				"Attempt to get component but container doesnt exist.");
			Ptr<Container<ComponentType>> container = maybeContainer.value();
			const ComponentIndex componentIndex = entityComponents.GetComponentIndex<ComponentType>();
			OS::AssertMessage(
				container->IsComponentExist(componentIndex), 
				"Attempt to get component that doesnt exist.");
			return (*container)[componentIndex];
		}

		template<class ComponentType>
		[[nodiscard]]
		const ComponentType* GetComponent(Entity::Id entityId) const noexcept {
			return const_cast<ComponentsManager*>(this)->GetComponent<ComponentType>(entityId);
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) const noexcept {
			return GetEntityComponents(entityId).IsComponentExist<ComponentType>();
		}

	private:

		class IContainer {
		public:

			IContainer(ComponentTypeId typeId) noexcept : typeId_{ typeId } { }

			virtual std::size_t GetSize() const noexcept = 0;
			ComponentTypeId GetTypeId() const noexcept { return typeId_; }

			//virtual void AddDelayedComponents() = 0;

		private:
			ComponentTypeId typeId_;
		};
		using ComponentIndex = std::size_t;

		template<class ComponentType>
		class Container final : public IContainer {
		public:

			Container() noexcept : IContainer{ ComponentType::GetTypeId() } { }

			[[nodiscard]]
			virtual Common::Size GetSize() const noexcept override {
				return components_.size();
			}

			[[nodiscard]]
			ComponentType* operator[](ComponentIndex index) noexcept {
				OS::AssertMessage(
					!IsComponentIndexFree(index),
					"Attempt to get component by index but component with this index was removed.");
				OS::AssertMessage(
					index < components_.size(),
					"Attempt to use incorrect component index.");
				return &components_[index];
			}

			[[nodiscard]]
			const ComponentType* operator[](ComponentIndex index) const noexcept {
				return const_cast<Container<ComponentType>*>(this)->operator[](index);
			}

			template<class ...Args>
			[[nodiscard]]
			ComponentIndex CreateComponent(Args&&... args) noexcept {
				components_.emplace_back(std::forward<Args>(args)...);
				return components_.size() - 1;
			}

			[[nodiscard]]
			bool IsComponentExist(ComponentIndex index) const noexcept {
				return (index < components_.size() && !IsComponentIndexFree(index));
			}

			[[nodiscard]]
			bool IsComponentIndexValid(ComponentIndex index) const noexcept {
				return (index < components_.size() || IsComponentIndexFree(index));
			}

			void RemoveComponent(ComponentIndex index) noexcept {
				ComponentType* component = this->operator[](index);
				component->~ComponentType();
				freeIndices_.push_back(index);
			}

		private:

			[[nodiscard]]
			bool IsComponentIndexFree(ComponentIndex index) const noexcept {
				return std::find(freeIndices_.cbegin(), freeIndices_.cend(), index) == freeIndices_.cend();
			} 

		private:

			std::vector<ComponentIndex> freeIndices_;
			std::vector<ComponentType> components_;
		};

		template<class ComponentType>
		[[nodiscard]]
		Ptr<Container<ComponentType>> GetCreateContainer() noexcept {
			Maybe<Ptr<Container<ComponentType>>> maybeComponentContainer = GetContainer<ComponentType>();
			if (maybeComponentContainer.has_value()) {
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

		class EntityComponents final {
		public:

			EntityComponents() noexcept = default;

			[[nodiscard]]
			std::size_t GetComponentsNumber() const noexcept {
				return typeIndex_.size();
			}

			template<class ComponentType>
			[[nodiscard]]
			bool IsComponentExist() const noexcept {
				const auto it = typeIndex_.find(ComponentType::GetTypeId());
				return it != typeIndex_.cend();
			}

			template<class ComponentType>
			[[nodiscard]]
			ComponentIndex GetComponentIndex() const noexcept {

				const ComponentTypeId componentTypeId = ComponentType::GetTypeId();
				const ComponentIndex componentIndex = typeIndex_.find(componentTypeId)->second;

				return componentIndex;
			}

			template<class ComponentType>
			void AddComponent(ComponentIndex componentIndex) noexcept {

				ComponentTypeId componentTypeId = ComponentType::GetTypeId();
				typeIndex_.insert(std::pair{ componentTypeId, componentIndex });

			}

			template<class ComponentType>
			void RemoveComponent() noexcept {

				ComponentTypeId componentTypeId = ComponentType::GetTypeId();
				typeIndex_.erase(componentTypeId);

			}

		private:

			//Component type to component index.
			std::map<ComponentTypeId, ComponentIndex> typeIndex_;

		};

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
			return (entityComponents_.find(entityId) != entityComponents_.end());
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
			OS::AssertMessage(IsEntityComponentsExist(entityId), "Attempt to get entity components, but entity doesn't have them.");
			auto it = entityComponents_.find(entityId);
			return it->second;
		}

		[[nodiscard]]
		const EntityComponents& GetEntityComponents(Entity::Id entityId) const noexcept {
			return const_cast<ComponentsManager*>(this)->GetEntityComponents(entityId);
		}

		[[nodiscard]]
		EntityComponents& CreateEntityComponents(Entity::Id entityId) noexcept {
			OS::AssertMessage(!IsEntityComponentsExist(entityId), "Attempt to create entity components structure when it exists yet.");
			entityComponents_.insert({ entityId, EntityComponents{} });
			return GetEntityComponents(entityId);
		}

		std::map<Entity::Id, EntityComponents> entityComponents_;
		std::map<ComponentTypeId, std::shared_ptr<IContainer>> componentContainer_;
	};


}