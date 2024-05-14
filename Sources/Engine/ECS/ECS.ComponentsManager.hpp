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
			Ptr<Container<ComponentType>> container = GetCreateContainer<ComponentType>();
			const ComponentIndex index = container->CreateComponent(std::forward<Args>(args)...);
			EntityComponents& entityComponents = GetCreateEntityComponents(entityId);
			entityComponents.AddComponent<ComponentType>(index);
		}

		template<class ComponentType>
		[[nodiscard]]
		ComponentType* GetComponent(Entity::Id entityId) noexcept {
			Maybe<Ptr<Container<ComponentType>>> maybeContainer = GetContainer<ComponentType>();
			if (maybeContainer.has_value()) {
				Ptr<Container<ComponentType>> container = maybeContainer.value();
				auto entityComponentsIt = entityComponents_.find(entityId);
				if (entityComponentsIt->second.IsComponentExist<ComponentType>()) {
					const ComponentIndex componentIndex = entityComponentsIt->second.GetComponentIndex<ComponentType>();
					ComponentType* component = (*container)[componentIndex];
					return component;
				} else {
					return nullptr;
				}
			} else {
				return nullptr;
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) const noexcept {
			return (GetComponent<ComponentType>() != nullptr);
		}

	private:

		class IContainer {
		public:

			IContainer(ComponentTypeId typeId) noexcept : typeId_{ typeId } { }

			virtual std::size_t GetSize() const noexcept = 0;
			ComponentTypeId GetTypeId() const noexcept { return typeId_; }

		private:
			ComponentTypeId typeId_;
		};
		using ComponentIndex = std::size_t;

		template<class ComponentType>
		class Container final : public IContainer {
		public:

			Container() noexcept : IContainer{ ComponentType::GetTypeId() } { }

			[[nodiscard]]
			virtual std::size_t GetSize() const noexcept override {
				return components_.size();
			}

			[[nodiscard]]
			ComponentType* operator[](std::size_t index) noexcept {
				return &components_[index];
			}

			template<class ...Args>
			[[nodiscard]]
			ComponentIndex CreateComponent(Args&&... args) noexcept {
				components_.emplace_back(std::forward<Args>(args)...);
				return components_.size() - 1;
			}

		private:
			std::vector<ComponentType> components_;
		};

		template<class ComponentType>
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
			ComponentIndex GetComponentIndex() noexcept {

				//const ComponentTypeId componentTypeId = ComponentType::GetTypeId();
				//const ComponentIndex componentIndex = typeIndex_[componentTypeId];

				return 0;//componentIndex;// typeIndex_[ComponentType::GetTypeId()];
			} 

			template<class ComponentType>
			void AddComponent(ComponentIndex componentIndex) noexcept {

				ComponentTypeId componentTypeId = ComponentType::GetTypeId();
				typeIndex_.insert(std::pair{ componentTypeId, componentIndex });
				
			}

		private:
			  
			//Component type to component index.
			std::map<ComponentTypeId, ComponentIndex> typeIndex_;

		}; 

		[[nodiscard]]
		EntityComponents& GetCreateEntityComponents(Entity::Id entityId) noexcept {
			if (IsEntityComponentsExist(entityId)) {
				return GetEntityComponents(entityId);
			} else {
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
			} else {
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
		EntityComponents& CreateEntityComponents(Entity::Id entityId) noexcept {
			OS::AssertMessage(!IsEntityComponentsExist(entityId), "Attempt to create entity components structure when it exists yet.");
			entityComponents_.insert({ entityId, EntityComponents{} });
			return GetEntityComponents(entityId);
		}

		std::map<Entity::Id, EntityComponents> entityComponents_;
		std::map<ComponentTypeId, std::shared_ptr<IContainer>> componentContainer_;
	};


}