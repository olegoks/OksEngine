#pragma once 

#include <utility>
#include <memory>
#include <mutex>
#include <ECS2.Entity.hpp>
#include <ECS2.SystemsManager.hpp>
#include <ECS2.ComponentsManager.hpp>
#include <ECS2.EntitiesManager.hpp>
#include <ECS2.Archetype.hpp>

namespace ECS2 {

	class [[nodiscard]] World final {
	public:

		World() {

			for (
				Entity::Id entityId = 0;
				entityId < 100'000;
				entityId++) {
				freeEntityIds_.insert(entityId);
			}

		}

		using DelayedRequest = std::function<void()>;

		Entity::Id GetFreeEntityId() {
			auto freeIdIt = freeEntityIds_.begin();
			const Entity::Id freeId = *freeIdIt;
			freeEntityIds_.erase(freeIdIt);
			return freeId;
		}

		void FreeEntityId(Entity::Id entityId) {

#pragma region Assert
			OS::AssertMessage(!freeEntityIds_.contains(entityId), 
				"");
#pragma endregion

			freeEntityIds_.insert(entityId);
		}

		//Create archetype entity.
		template<class ...Components>
		requires (sizeof...(Components) > 0)
		[[nodiscard]]
		Entity::Id CreateEntity() noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			const Entity::Id entityId = GetFreeEntityId();
			requests_.emplace_back([entityId, this]() mutable {
				ComponentsFilter archetypeComponentsFilter;
				archetypeComponentsFilter.SetBits<Components...>();
				archetypeEntitiesComponents_[entityId] = archetypeComponentsFilter;

				auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);
				const bool isArchetypeComponentsContainersCreated = archetypeComponentsIt != archetypeComponents_.end();
				std::shared_ptr<ArchetypeComponents<Components...>> archetypeComponents = nullptr;
				if (!isArchetypeComponentsContainersCreated) {
					archetypeComponents = std::make_shared<ArchetypeComponents<Components...>>(100);
					archetypeComponents_[archetypeComponentsFilter] = archetypeComponents;
				}
				else {
					std::shared_ptr<IArchetypeComponents> iArchetypeComponents = archetypeComponentsIt->second;
					archetypeComponents = std::dynamic_pointer_cast<ArchetypeComponents<Components...>>(iArchetypeComponents);
				}
				archetypeComponents->CreateEntity(entityId);
				});
			return entityId;
		}

		Entity::Id CreateEntity() noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			const Entity::Id entityId = GetFreeEntityId();
			requests_.emplace_back([this, entityId]() mutable {
				dynamicEntitiesComponentFilters_[entityId] = ComponentsFilter{};
				});
			return entityId;
		}

		void DestroyEntity(Entity::Id entityId) noexcept {
			std::lock_guard lock{ addRequestMutex_ };

			requests_.emplace_back([this, entityId]() mutable {
				//Archetype entity.
				if (archetypeEntitiesComponents_.contains(entityId)) {
					const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
					std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponents_[componentsFilter];
					archetypeComponents->DestroyEntity(entityId);
					archetypeEntitiesComponents_.erase(entityId);
				}
				else { 

#pragma region Assert
					OS::AssertMessage(
						dynamicEntitiesComponentFilters_.contains(entityId),
						"");
#pragma endregion

					const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];

					//Remove entity components in containers.
					componentsFilter.ForEachSetComponent([&](ComponentTypeId componentTypeId) {

#pragma region Assert
						OS::AssertMessage(
							dynamicEntitiesContainers_.contains(componentTypeId),
							"Attempt to get component that doesn't exist.");
#pragma endregion

						std::shared_ptr<IContainer> container = dynamicEntitiesContainers_[componentTypeId];
						container->RemoveComponent(entityId);
						});

					dynamicEntitiesComponentFilters_.erase(entityId);


				}
				FreeEntityId(entityId);
				});

			
		}

		template<class ...Components>
		requires (sizeof...(Components) > 0)
		bool IsEntityExist() {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();

			//TODO: Optimize
			for (auto& [entityId, componentsFilter] : dynamicEntitiesComponentFilters_) {
				if (componentsFilter.IsSet<Components...>()) {
					return true;
				}
			}

			for (auto& entity : archetypeEntitiesComponents_) {

				if (componentsFilter.IsSubsetOf(entity.second)) {
					auto archetypeComponentsIt = archetypeComponents_.find(entity.second);
					const bool isArchetypeComponentsContainersCreated = archetypeComponentsIt != archetypeComponents_.end();
					if (!isArchetypeComponentsContainersCreated) {
						continue;
					}
					std::shared_ptr<IArchetypeComponents> iArchetypeComponents = archetypeComponentsIt->second;
					if(iArchetypeComponents->IsComponentsExist<Components...>(entity.first)){
						return true;
					}
				}
			}
			return false;
		}

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			requests_.emplace_back([entityId, componentCreateArgs = std::make_tuple(std::forward<Args>(args)...), this]() mutable {
				std::apply([&](auto&&... unpackedArgs) {
					//Archetype entity.
					if (archetypeEntitiesComponents_.contains(entityId)) {

						const ComponentsFilter archetypeComponentsFilter = archetypeEntitiesComponents_[entityId];
#pragma region Assert
						OS::AssertMessage(archetypeComponentsFilter.IsSet<ComponentType>(),
							"Attempt to add component to an archetype entity "
							"that can't contain this component");
#pragma endregion
						auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);
						std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponentsIt->second;
						archetypeComponents->CreateComponent<ComponentType>(entityId, unpackedArgs...);
					}
					else {

#pragma region Assert
						OS::AssertMessage(
							!dynamicEntitiesComponentFilters_[entityId].IsSet<ComponentType>(),
							"Attempt to add component to the entity that was already added.");
#pragma endregion

						auto container = std::dynamic_pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
						if (container == nullptr) {
							auto newContainer = std::make_shared<Container<ComponentType>>();
							dynamicEntitiesContainers_[ComponentType::GetTypeId()] = newContainer;
							container = newContainer;
						}
						container->CreateComponent(entityId, unpackedArgs...);
						dynamicEntitiesComponentFilters_[entityId].SetBits<ComponentType>();
					}
					}, componentCreateArgs);
				});
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId) noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			requests_.emplace_back([entityId, this]() mutable {
				//Archetype entity.
				if (archetypeEntitiesComponents_.contains(entityId)) {
					const ComponentsFilter archetypeComponentsFilter = archetypeEntitiesComponents_[entityId];
#pragma region Assert
					OS::AssertMessage(archetypeComponentsFilter.IsSet<ComponentType>(),
						"Attempt to add component to an archetype entity "
						"that can't contain this component");
#pragma endregion
					auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);
					std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponentsIt->second;
					archetypeComponents->RemoveComponent<ComponentType>(entityId);
				}
				else {
#pragma region Assert
					OS::AssertMessage(dynamicEntitiesContainers_.contains(ComponentType::GetTypeId()),
						"Attempt to remove component, but component container doesn't exist.");
					OS::AssertMessage(dynamicEntitiesComponentFilters_.contains(entityId),
						"Attempt to remove entity component, but entity doesn't exist.");
					OS::AssertMessage(dynamicEntitiesComponentFilters_[entityId].IsSet<ComponentType>(),
						"Attempt ot remove entity component, but entity doesn't contain this component.");
#pragma endregion
					auto container = std::dynamic_pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
					container->RemoveComponent(entityId);
					dynamicEntitiesComponentFilters_[entityId].RemoveBits<ComponentType>();
				}
				});
		}

		template<class ComponentType>
		[[nodiscard]]
		ComponentType* GetComponent(Entity::Id entityId) noexcept {
			//Archetype entity.
			if (archetypeEntitiesComponents_.contains(entityId)) {
				const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
				std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponents_[componentsFilter];
				return archetypeComponents->GetComponent<ComponentType>(entityId);
			}
			else {
#pragma region Assert
				OS::AssertMessage(
					dynamicEntitiesContainers_.contains(ComponentType::GetTypeId()),
					"Attempt to get component that doesn't exist.");
#pragma endregion
				auto container = std::dynamic_pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
				return container->GetComponent(entityId);
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) noexcept {
			if(archetypeEntitiesComponents_.contains(entityId))
			{
				//TODO: if archetype components filter contains component it doesn't mean that archetype has this component.
				//Need to check archetype container.
				const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
				if (componentsFilter.IsSet<ComponentType>()) {
					return true;
				}
			}
			if(dynamicEntitiesComponentFilters_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];
				if (componentsFilter.IsSet<ComponentType>()) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]]
		ComponentsFilter GetEntityComponentsFilter(Entity::Id entityId) noexcept {
			if (archetypeEntitiesComponents_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
				return componentsFilter;
			}
			if (dynamicEntitiesComponentFilters_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];
				return componentsFilter;
			}
			OS::AssertFailMessage("Entity with such id is doesnt exist.");
			return ComponentsFilter{};
		}



		template<class ...Components>
		using SystemUpdateFunction = std::function<void(Entity::Id, Components*...)>;

		template<typename First, typename... Rest>
		struct FirstType {
			using type = First;
		};

		template<typename... Args>
		using FirstT = typename FirstType<Args...>::type;

		template<typename First, typename... Rest>
		auto make_tuple_excluding_first(First, Rest&... rest) {
			return std::make_tuple(rest...);
		}


		//Process all entities.
		//TODO: optimize
		template<class ...Components>
		void ForEachEntity(std::function<void(Entity::Id)>&& processEachEntityId) {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();
			for (const auto& [entityId, componentsFilter] : archetypeEntitiesComponents_) {
				if (componentsFilter.IsSubsetOf(componentsFilter)) {
					processEachEntityId(entityId);
				}
			}
			for (const auto& [entityId, componentsFilter] : dynamicEntitiesComponentFilters_) {
				if (componentsFilter.IsSubsetOf(componentsFilter)) {
					processEachEntityId(entityId);
				}
			}
		}

		template<class ...Components>
		void ForEachEntity(const ComponentsFilter& excludes, SystemUpdateFunction<Components...>&& processEntity) {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();

			//Find archetype that corresponds to components filter.

			for (const auto& [archetypeComponentsFilter, archetypeComponents] : archetypeComponents_) {
				if (componentsFilter.IsSubsetOf(archetypeComponentsFilter)) {
					//TODO: Add exclude.
					archetypeComponents->ForEachEntity<Components...>(excludes, std::forward<SystemUpdateFunction<Components...>&&>(processEntity));
				}
			}

			//Process dynamic entities.
			auto containers = std::make_tuple(std::dynamic_pointer_cast<Container<Components>>(dynamicEntitiesContainers_[Components::GetTypeId()])...);
			bool allContainersExist = false;
			std::apply([&](auto&&... containers) {
				allContainersExist = ((containers != nullptr) && ...);
				}, containers);
			if (allContainersExist) {

				auto firstContainer = std::get<0>(containers);
				if constexpr (sizeof...(Components) == 1) {
					firstContainer->ForEachComponent(
						[&](FirstT<Components...>* component, Entity::Id entityId) {
							const ComponentsFilter entityComponentsFilter = dynamicEntitiesComponentFilters_[entityId];
							if (componentsFilter.IsSubsetOf(entityComponentsFilter) && entityComponentsFilter.IsNotSet(excludes)) {
								processEntity(entityId, component);
							}
						});
				}
				else {
					auto containersExcludeFirst = make_tuple_excluding_first(std::get<std::shared_ptr<Container<Components>>>(containers)...);
					firstContainer->ForEachComponent(
						[&](FirstT<Components...>* component, Entity::Id entityId) {
							const ComponentsFilter entityComponentsFilter = dynamicEntitiesComponentFilters_[entityId];
							if (componentsFilter.IsSubsetOf(entityComponentsFilter) && entityComponentsFilter.IsNotSet(excludes)) {
								std::apply([&](auto&&... containers) {
									processEntity(entityId, component, containers->GetComponent(entityId)...);
									}, containersExcludeFirst);
							}
						});
				}
			}


		}

		void StartFrame() {
			ApplyDelayedRequests();
		}

		void EndFrame() {
			ApplyDelayedRequests();
		}

		void ApplyDelayedRequests() {
			for (auto& addition : requests_) {
				addition();
			}
			requests_.clear();
		}
	private:

		std::mutex addRequestMutex_;
		std::vector<DelayedRequest> requests_;

		std::set<Entity::Id, std::less<Entity::Id>> freeEntityIds_;

		//Dynamic entities.
		std::map<Entity::Id, ComponentsFilter> dynamicEntitiesComponentFilters_; //Components filter contains current entity components.
		std::map<ComponentTypeId, std::shared_ptr<IContainer>> dynamicEntitiesContainers_;

		//Archetype components.
		std::map<Entity::Id, ComponentsFilter> archetypeEntitiesComponents_; // Components filter contains archetype components.
		std::map<
			ComponentsFilter,
			std::shared_ptr<IArchetypeComponents>,
			ComponentsFilter::LessComparator> archetypeComponents_;

	};
}