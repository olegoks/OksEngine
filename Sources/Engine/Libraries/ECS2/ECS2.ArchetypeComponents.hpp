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
			ASSERT_FMSG(containers_.contains(Component::GetTypeId()),
				"Attempt to get container in the archetype components structure that doesn't exist.");
#pragma endregion
			std::shared_ptr<IArchetypeContainer> container = containers_[Component::GetTypeId()];
			return std::dynamic_pointer_cast<ArchetypeContainer<Component>>(container);
		}

		Common::Size GetEntitiesNumber() const noexcept {
			//AllContainers must have the same size.
			const Common::Size size = containers_.begin()->second->GetReservedSize();
#if !defined(NDEBUG)
			for (auto& [componentTypeId, container] : containers_) {
				ASSERT_MSG(container->GetReservedSize() == size, "All archetype containers must have the same size.");
			}
#endif

			return entityIdComponentIndex_.size();
		}

		void CreateEntity(Entity::Id entityId) {
			if (freeComponentIndices_.empty()) {
				//All buffers have same size.
				const Common::Size previousSize = containers_.begin()->second->GetReservedSize();
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
			entityIdComponentIndex_[entityId] = *freeComponentIndexIt;
			freeComponentIndices_.erase(freeComponentIndexIt);
		}

		void DestroyEntity(Entity::Id entityId) {

			const ComponentIndex componentIndex = entityIdComponentIndex_[entityId];

#pragma region Assert
			ASSERT_FMSG(
				entityIdComponentIndex_.contains(entityId), 
				"");
			ASSERT_FMSG(
				!freeComponentIndices_.contains(componentIndex),
				"");
#pragma endregion

			componentIndexEntityId_[componentIndex] = Entity::Id::invalid_;
			entityIdComponentIndex_.erase(entityId);
			freeComponentIndices_.insert(componentIndex);
		}

		template<class Component>
		inline bool IsComponentExist(Entity::Id entityId) {
			return entityIdComponentsFilter_[entityId].IsSet<Component>();
		}

		ComponentsFilter GetComponentsFilter(Entity::Id entityId) {
			return entityIdComponentsFilter_[entityId];
		}

		template<class ...Components>
		inline bool IsComponentsExist(Entity::Id entityId) {
			return entityIdComponentsFilter_[entityId].IsSet<Components...>();
		}

		template<class Component, class ...Args>
		inline void CreateComponent(Entity::Id entityId, Args&&... args) {
			const ComponentIndex entityComponentIndex = entityIdComponentIndex_[entityId];
			auto container = GetContainer<Component>();
			container->CreateComponent<Component>(entityComponentIndex, args...);
			entityIdComponentsFilter_[entityId].SetBits<Component>();
		}

		template<class Component>
		inline void RemoveComponent(Entity::Id entityId) {
			auto container = GetContainer<Component>();
#pragma region Assert
			ASSERT_FMSG(entityIdComponentIndex_.contains(entityId), "");
#pragma endregion
			entityIdComponentsFilter_[entityId].RemoveBits<Component>();
		}

		template<class Component>
		inline Component* GetComponent(Entity::Id entityId) {
#pragma region Assert
			ASSERT_FMSG(entityIdComponentIndex_.contains(entityId),
				"Attempt to get component of entity that doesn't exist.");
#pragma endregion
			const ComponentIndex componentIndex = entityIdComponentIndex_[entityId];
#pragma region Assert
			ASSERT_FMSG(componentIndex != invalidComponentIndex_,
				"Attempt to get component of entity but component doesn't exist.");
#pragma endregion
			auto container = std::dynamic_pointer_cast<ArchetypeContainer<Component>>(containers_[Component::GetTypeId()]);
			return (*container)[componentIndex];
		}
		



		// Get certain components set of  the definite entity
		// Archetype entity can not contain need component, in the case we will return nullptr.
		// ComponentX	ComponentY  ComponentZ	EntityId
		//	[		]	[		]	[		]	23
		//	[		]	[		]	[		]	13
		//	[		]	[		]	[		]	Invalid
		//	[		]	[		]	[		]	20	<- For example we can get ComponentX and Component Z of entity with id 20
		//	[		]	[		]	[		]	21
		//	[		]	[		]	[		]	22
		//	[		]	[		]	[		]	23
		template<class ...Components>
		inline std::tuple<Components*...> GetComponents(Entity::Id entityId) {

			const ComponentIndex componentIndex = entityIdComponentIndex_[entityId];

			return std::make_tuple(
				[this, componentIndex]() -> Components* {

					auto containerIt = containers_.find(Components::GetTypeId());
					if (containerIt != containers_.end()) {
						
						auto container = std::pointer_cast<ArchetypeContainer<Components>>(containerIt->second);
						ASSERT_FMSG(container != nullptr,
							"Failed to cast to appropriate container type");
						return (*container)[componentIndex];
					} else {
						//Return nullptr if archetype entity doesnt contain need component.
						return nullptr;
					}
				}()...
			);
		}


		// Get certain component pointers definite archetype
		// Archetype entity can not contain some components, component will initialized with default constructor.
		// 
		//   ptr to					   ptr to
		//	   |						 |
		//	  \ /						\ /
		// ComponentX	ComponentY  ComponentZ	EntityId
		//	[		]	[		]	[		]	23
		//	[		]	[		]	[		]	13
		//	[ def	]	[ def	]	[ def	]	Invalid
		//	[		]	[		]	[		]	20
		//	[		]	[		]	[		]	21
		//	[		]	[		]	[		]	22
		//	[		]	[		]	[		]	23

		template<class ...Components>
		inline std::tuple<Entity::Id*, Components*...> GetComponents() {
			return std::make_tuple(
				componentIndexEntityId_.data(),
				[this]() -> Components* {
					if (containers_.contains(Components::GetTypeId())) {
						auto container = GetContainer<Components>();
						return container->GetComponents();
					}
					else {
						return nullptr;
					}
					
				}()...
			);
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
			ASSERT_FMSG(!containers_.contains(ComponentType::GetTypeId()),
				"Attempt to create container in the archetype components structure that exist.");
#pragma endregion
			containers_[ComponentType::GetTypeId()] = std::make_shared<ArchetypeContainer<ComponentType>>(startSize);
		}

		std::vector<Entity::Id> componentIndexEntityId_; // vector index - component index, value - entity id.
		std::unordered_map<Entity::Id, ComponentIndex, Entity::Id::Hash> entityIdComponentIndex_;
		std::unordered_map<Entity::Id, ComponentsFilter, Entity::Id::Hash> entityIdComponentsFilter_; // quick solution, need to create EntityInfo to use only one map with key entityId;
		std::set<ComponentIndex, std::less<ComponentIndex>> freeComponentIndices_;
		std::unordered_map<ComponentTypeId, std::shared_ptr<IArchetypeContainer>> containers_;

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