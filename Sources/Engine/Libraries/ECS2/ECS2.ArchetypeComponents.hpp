#pragma once 

#include <map>
#include <set>

#include <ECS2.Common.hpp>
#include <ECS2.Component.hpp>
#include <ECS2.Container.hpp>
#include <ECS2.Entity.hpp>

namespace ECS2 {

	class IArchetypeComponents {
	public:

		using EntityIndex = Common::Index;

		IArchetypeComponents() { }

		template<class Component>
		inline std::shared_ptr<ArchetypeContainer<Component>> GetContainer() {
#pragma region Assert
			OS::AssertMessage(containers_.contains(Component::GetTypeId()),
				"Attempt to get container in the archetype components structure that doesn't exist.");
#pragma endregion
			std::shared_ptr<IContainer> container = containers_[Component::GetTypeId()];
			return std::dynamic_pointer_cast<ArchetypeContainer<Component>>(container);
		}

		void CreateEntity(Entity::Id entityId) {
			if (freeComponentIndices_.empty()) {
				//All buffers have same size.
				const Common::Size previousSize = containers_.begin()->second->GetSize();
				const Common::Size newSize = previousSize + (previousSize / 2);
				for (auto& [componentTypeId, container] : containers_) {
					container->Resize(newSize);
					componentIndexEntityId_.resize(newSize, Entity::Id::invalid_);
				}
				for (
					Common::Size freeIndex = previousSize; 
					freeIndex < newSize; 
					freeIndex++) {
					freeComponentIndices_.insert(freeIndex);
				}
			}
			auto freeComponentIndexIt = freeComponentIndices_.begin();
			componentIndexEntityId_[*freeComponentIndexIt] = entityId;
			freeComponentIndices_.erase(freeComponentIndexIt);
		}

		template<class Component, class ...Args>
		inline void CreateComponent(Entity::Id entityId, Args&&... args) {
			const ComponentIndex entityComponentIndex = entityIdComponentIndex_[entityId];
			auto container = GetContainer<Component>();
			container->CreateComponent<Component>(entityComponentIndex, args...);
			entityIdComponentsFilter_[entityId].SetBits<Component>();
		}

		template<class Component, class ...Args>
		inline void RemoveComponent(Entity::Id entityId, Args&&... args) {
			auto container = GetContainer<Component>();
#pragma region Assert
			OS::AssertMessage(entityIdComponentIndex_.contains(entityId), "");
#pragma endregion
			entityIdComponentsFilter_[entityId].RemoveBits<Component>();
		}

		template<class Component>
		inline Component* GetComponent(Entity::Id entityId) {
#pragma region Assert
			OS::AssertMessage(entityIdComponentIndex_.contains(entityId),
				"Attempt to get component of entity that doesn't exist.");
#pragma endregion
			const ComponentIndex componentIndex = entityIdComponentIndex_[entityId];
#pragma region Assert
			OS::AssertMessage(componentIndex != invalidComponentIndex_,
				"Attempt to get component of entity but component doesn't exist.");
#pragma endregion
			auto container = std::dynamic_pointer_cast<ArchetypeContainer<Component>>(containers_[Component::GetTypeId()]);
			return (*container)[componentIndex];
		}

		template<class ...Components>
		inline void ForEachEntity(const ComponentsFilter& exclude, std::function<void(Entity::Id, Components*...)>&& processEntity) {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();

			auto containers = std::make_tuple(GetContainer<Components>()...);
			for (
				Common::Index componentIndex = 0;
				componentIndex < componentIndexEntityId_.size();
				++componentIndex) {
				const Entity::Id entityId = componentIndexEntityId_[componentIndex];
				if (entityId != Entity::Id::invalid_) { // if entity exist in this component index
					const ComponentsFilter entityComponentsFilter = entityIdComponentsFilter_[entityId];
					if (componentsFilter.IsSubsetOf(entityComponentsFilter) && entityComponentsFilter.IsNotSet(exclude)) { // if entity has all need components.
						processEntity(entityId, (*std::get<std::shared_ptr<ArchetypeContainer<Components>>>(containers))[componentIndex]...);
					}
				}
			}
		}

		virtual ~IArchetypeComponents() = default;

	protected:

		template<class ComponentType>
		inline void CreateContainer(Common::Size startSize) {
#pragma region Assert
			OS::AssertMessage(!containers_.contains(ComponentType::GetTypeId()),
				"Attempt to create container in the archetype components structure that exist.");
#pragma endregion
			containers_[ComponentType::GetTypeId()] = std::make_shared<ArchetypeContainer<ComponentType>>(startSize);
		}

		std::vector<Entity::Id> componentIndexEntityId_; // vector index - component index, value - entity id.
		std::map<Entity::Id, ComponentIndex> entityIdComponentIndex_;
		std::map<Entity::Id, ComponentsFilter> entityIdComponentsFilter_; // quick solution, need to create EntityInfo to use only one map with key entityId;
		std::set<ComponentIndex, std::less<ComponentIndex>> freeComponentIndices_;
		std::map<ComponentTypeId, std::shared_ptr<IContainer>> containers_;

	};

	template<class ...Components>
	class ArchetypeComponents : public IArchetypeComponents {
	public:
		ArchetypeComponents(Common::Size startSize) : IArchetypeComponents{}  {
			(CreateContainer<Components>(startSize), ...);
			componentIndexEntityId_.resize(startSize, Entity::Id::invalid_);
			for (Common::Index i = 0; i < startSize; i++) {
				freeComponentIndices_.insert(i);
			}
		}

	private:

	};

}