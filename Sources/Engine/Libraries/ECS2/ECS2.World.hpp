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
				entityId < 200'000;
				entityId++) {
				freeEntityIds_.insert(entityId);
			}

			dynamicEntitiesComponentFilters_.max_load_factor(0.5);
			dynamicEntitiesComponentFilters_.reserve(1000);

			dynamicEntitiesContainers_.max_load_factor(0.5);
			dynamicEntitiesContainers_.reserve(ComponentsFilter::GetMaxComponentsNumber());

			archetypeEntitiesComponents_.max_load_factor(0.5);
			archetypeEntitiesComponents_.reserve(30000);

			archetypeComponents_.max_load_factor(0.5);
			archetypeComponents_.reserve(ComponentsFilter::GetMaxComponentsNumber());
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
			ASSERT_FMSG(!freeEntityIds_.contains(entityId),
				"");
#pragma endregion

			freeEntityIds_.insert(entityId);
		}

		[[nodiscard]]
		//So we have delayed entities creation logic and if we created entity and in current frame trying to get entity type 
		//we will return Entity::Type::Undefined because we dont know type at the moment. 
		Entity::Type GetEntityType(Entity::Id entityId) const noexcept {

			ASSERT_FMSG(entityId.IsValid(), "Attempt to get type if entity with invalid id.");

			if (archetypeEntitiesComponents_.contains(entityId)) {
				return Entity::Type::Archetype;
			}
			else if (dynamicEntitiesComponentFilters_.contains(entityId)) {
				return Entity::Type::Dynamic;
			}

			return Entity::Type::Undefined;
		}

		[[nodiscard]]
		bool IsArchetypeEntity(Entity::Id entityId) const noexcept {
			return GetEntityType(entityId) == ECS2::Entity::Type::Archetype;
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
					archetypeComponents = Common::pointer_cast<ArchetypeComponents<Components...>>(iArchetypeComponents);
				}
				archetypeComponents->CreateEntity(entityId);
				});
			return entityId;
		}

		//Create archetype entity by components filer. So you must preallocate containers if you want to use this method.
		[[nodiscard]]
		Entity::Id CreateEntity(ComponentsFilter archetypeComponentsFilter) noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			const Entity::Id entityId = GetFreeEntityId();
			requests_.emplace_back([entityId, archetypeComponentsFilter, this]() mutable {
				archetypeEntitiesComponents_[entityId] = archetypeComponentsFilter;

				auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);

				ASSERT_FMSG(archetypeComponentsIt != archetypeComponents_.end(),
					"Attempt to create archetype entity that is not preallocated. Archetype: {}",
					archetypeComponentsFilter.GetComponentsList());

				std::shared_ptr<IArchetypeComponents> iArchetypeComponents = archetypeComponentsIt->second;
				iArchetypeComponents->CreateEntity(entityId);
				});
			return entityId;
		}

		template<class ...Components>
			requires (sizeof...(Components) > 0)
		void PreallocateArchetypeEntities() noexcept {

			std::lock_guard lock{ addRequestMutex_ };
			requests_.emplace_back([this]() mutable {

				ComponentsFilter archetypeComponentsFilter;
				archetypeComponentsFilter.SetBits<Components...>();
				auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);

				ASSERT_MSG(
					archetypeComponentsIt == archetypeComponents_.end(),
					"Attempt to preallocate archetype that allocated yet.");

				auto archetypeComponents = std::make_shared<ArchetypeComponents<Components...>>(100);
				archetypeComponents_[archetypeComponentsFilter] = archetypeComponents;

				});

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
					ASSERT_FMSG(
						dynamicEntitiesComponentFilters_.contains(entityId),
						"");
#pragma endregion

					const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];

					//Remove entity components in containers.
					componentsFilter.ForEachSetComponent([&](ComponentTypeId componentTypeId, bool isLast) {

#pragma region Assert
						ASSERT_FMSG(
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
					if (iArchetypeComponents->IsComponentsExist<Components...>(entity.first)) {
						return true;
					}
				}
			}
			return false;
		}

		bool IsEntityExist(Entity::Id entityId) const {
			//TODO: Optimize
			return
				dynamicEntitiesComponentFilters_.contains(entityId) ||
				archetypeEntitiesComponents_.contains(entityId);

		}



		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, const char* callerSystemName, Args&&... args) noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			ASSERT(entityId.IsValid());
			requests_.emplace_back([callerSystemName, entityId, componentCreateArgs = std::make_tuple(std::forward<Args>(args)...), this]() mutable {
				std::apply([&](auto&&... unpackedArgs) {
					//Archetype entity.
					if (archetypeEntitiesComponents_.contains(entityId)) {

						const ComponentsFilter archetypeComponentsFilter = archetypeEntitiesComponents_[entityId];

						ASSERT_FMSG(archetypeComponentsFilter.IsSet<ComponentType>(),
							"Attempt to add component {} by {} to an archetype|{}| entity with id {} "
							"that can't contain this component", 
							ComponentType::GetName(), 
							callerSystemName, 
							archetypeComponentsFilter.GetComponentsList(),
							entityId.GetRawValue());
						
						auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);
						std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponentsIt->second;
						archetypeComponents->CreateComponent<ComponentType>(entityId, unpackedArgs...);
					}
					else {
						ASSERT_FMSG([&]() {
							auto it = dynamicEntitiesComponentFilters_.find(entityId);
							return (it != dynamicEntitiesComponentFilters_.cend());
							}(),
							"Attempt to add component by system \"{}\" to the entity that was already added.", callerSystemName);

						ASSERT_FMSG(
							!dynamicEntitiesComponentFilters_[entityId].IsSet<ComponentType>(),
							"Attempt to add component by system \"{}\" to the entity that was already added.", callerSystemName);

						auto container = Common::pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
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

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			CreateComponent<ComponentType, Args...>(

				entityId, "Called not from a system, maybe user manually created component", std::forward<Args&&>(args)...);
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId, const char* callerSystemName) noexcept {
			std::lock_guard lock{ addRequestMutex_ };
			requests_.emplace_back([callerSystemName, entityId, this]() mutable {
				//Archetype entity.
				if (archetypeEntitiesComponents_.contains(entityId)) {
					const ComponentsFilter archetypeComponentsFilter = archetypeEntitiesComponents_[entityId];
#pragma region Assert
					ASSERT_FMSG(archetypeComponentsFilter.IsSet<ComponentType>(),
						"Attempt to add component to an archetype entity "
						"that can't contain this component");
#pragma endregion
					auto archetypeComponentsIt = archetypeComponents_.find(archetypeComponentsFilter);
					std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponentsIt->second;
					archetypeComponents->RemoveComponent<ComponentType>(entityId);
				}
				else {
#pragma region Assert
					ASSERT_FMSG(dynamicEntitiesContainers_.contains(ComponentType::GetTypeId()),
						"Attempt to remove component by system \"{}\", but component container doesn't exist.", callerSystemName);
					ASSERT_FMSG(dynamicEntitiesComponentFilters_.contains(entityId),
						"Attempt to remove entity component, but entity doesn't exist.");
					ASSERT_FMSG(dynamicEntitiesComponentFilters_[entityId].IsSet<ComponentType>(),
						"Attempt ot remove entity component by system \"{}\", but entity doesn't contain this component.", callerSystemName);
#pragma endregion
					auto container = Common::pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
					container->RemoveComponent(entityId);
					dynamicEntitiesComponentFilters_[entityId].RemoveBits<ComponentType>();
				}
				});
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId) noexcept {
			RemoveComponent<ComponentType>(entityId, "Called not from a system, maybe user manually removed component");
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
				ASSERT_FMSG(
					dynamicEntitiesContainers_.contains(ComponentType::GetTypeId()),
					"Attempt to get component that doesn't exist.");
#pragma endregion	
				auto containerIt = dynamicEntitiesContainers_.find(ComponentType::GetTypeId());
				if (containerIt != dynamicEntitiesContainers_.end()) {
					auto container = Common::pointer_cast<Container<ComponentType>>(containerIt->second);

					//TODO: remove taking value by operator[], take only by find and iterator to not create map pairs with empty container pointer.
					if (container != nullptr) {
						return container->GetComponent(entityId);
					}
					else {
						return nullptr;
					}
				}
				else {
					return nullptr;
				}

			}
		}


		//Archetype entity can not contain need component, in the case we will return nullptr.
		template<class ...Components>
		[[nodiscard]]
		std::tuple<Components*...> GetComponents(Entity::Id entityId) noexcept {

			auto archetypeComponentsIt = archetypeEntitiesComponents_.find(entityId);
			if (archetypeComponentsIt != archetypeEntitiesComponents_.end()) {
				const ComponentsFilter componentsFilter = archetypeComponentsIt->second;
				std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponents_[componentsFilter];
				return archetypeComponents->GetComponents<Components...>(entityId);
			}
			else {
				return std::make_tuple(
					[this, entityId]() -> Components* {

						auto dynamicEntitiesContainerIt = dynamicEntitiesContainers_.find(Components::GetTypeId());
						if (dynamicEntitiesContainerIt != dynamicEntitiesContainers_.end()) {
							auto container = Common::pointer_cast<Container<Components>>(dynamicEntitiesContainerIt->second);
							return container->GetComponent(entityId);
						}
						else {
							return nullptr;
						}



					}()...
						);
			}
			return std::tuple<Components*...>{};
		}

		template<class ...Components>
		[[nodiscard]]
		std::tuple<Entity::Id*, Components*...> GetComponents() noexcept {

			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();
			auto archetypeComponentsIt = archetypeComponents_.find(componentsFilter);
			if (archetypeComponentsIt != archetypeComponents_.end()) {
				return archetypeComponentsIt->second->GetComponents<Components...>();
			}
			else {
				return std::make_tuple(
					nullptr,
					[]() -> Components* {
						return nullptr;
					}()...
						);
			}

		}

		template<class ...Components>
		[[nodiscard]]
		Common::Size GetEntitiesNumber() noexcept {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();
			auto archetypeComponentsIt = archetypeComponents_.find(componentsFilter);
			return (archetypeComponentsIt == archetypeComponents_.end()) ? (0) : (archetypeComponentsIt->second->GetEntitiesNumber());
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) noexcept {
			return IsComponentExist(entityId, ComponentType::GetTypeId());
		}

		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId, ComponentTypeId componentTypeId) noexcept {
			if (archetypeEntitiesComponents_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
				if (componentsFilter.IsSet(componentTypeId)) {
					return archetypeComponents_[componentsFilter]->IsComponentExist(entityId, componentTypeId);
				}
			}
			if (dynamicEntitiesComponentFilters_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];
				if (componentsFilter.IsSet(componentTypeId)) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]]
		//For archetypes returns existing entity components not archetype components.
		ComponentsFilter GetEntityComponentsFilter(Entity::Id entityId) noexcept {
			//TODO: optimize double map access.
			if (archetypeEntitiesComponents_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = archetypeEntitiesComponents_[entityId];
				return archetypeComponents_[componentsFilter]->GetComponentsFilter(entityId);
			}
			if (dynamicEntitiesComponentFilters_.contains(entityId))
			{
				const ComponentsFilter componentsFilter = dynamicEntitiesComponentFilters_[entityId];
				return componentsFilter;
			}
			ASSERT_FAIL_MSG("Entity with such id is doesnt exist.");
			return ComponentsFilter{};
		}

		[[nodiscard]]
		ComponentsFilter GetArchetypeComponents(Entity::Id entityId) noexcept {
			ASSERT_MSG(GetEntityType(entityId) == ECS2::Entity::Type::Archetype,
				"Attempt to get archetype components for not archetype entity.");

			return archetypeEntitiesComponents_[entityId];
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

			//{
			//	for (auto [archetypeComponentsFilter, archetypeContainer] : archetypeComponents_) {

			//	}
			//}

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
			//PIXBeginEvent(PIX_COLOR(255, 0, 0), "Search archetype entities.");
			for (const auto& [archetypeComponentsFilter, archetypeComponents] : archetypeComponents_) {
				if (componentsFilter.IsSubsetOf(archetypeComponentsFilter)) {
					//TODO: Add exclude.
					archetypeComponents->ForEachEntity<Components...>(excludes, std::forward<SystemUpdateFunction<Components...>&&>(processEntity));
				}
			}
			//PIXEndEvent();


			//Process dynamic entities.
			//PIXBeginEvent(PIX_COLOR(255, 0, 0), "Search dynamic entities.");
			auto containers = std::make_tuple(Common::pointer_cast<Container<Components>>(dynamicEntitiesContainers_[Components::GetTypeId()])...);
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
			//PIXEndEvent();

		}

		void StartFrame() {
			ApplyDelayedRequests();
		}

		void EndFrame() {
			ApplyDelayedRequests();
		}

		void ApplyDelayedRequests() {
			BEGIN_PROFILE("ECS2::World ApplyingDelayedRequests");
			for (auto& addition : requests_) {
				addition();
			}
			requests_.clear();
			END_PROFILE();
		}
	private:

		std::mutex addRequestMutex_;
		std::vector<DelayedRequest> requests_;

		std::set<Entity::Id, std::less<Entity::Id>> freeEntityIds_;

		//Dynamic entities.
		std::unordered_map<Entity::Id, ComponentsFilter, Entity::Id::Hash> dynamicEntitiesComponentFilters_; //Components filter contains current entity components.
		std::unordered_map<ComponentTypeId, std::shared_ptr<IContainer>> dynamicEntitiesContainers_;

		//Archetype components.
		std::unordered_map<Entity::Id, ComponentsFilter, Entity::Id::Hash> archetypeEntitiesComponents_; // Components filter contains archetype components.
		std::unordered_map<
			ComponentsFilter,
			std::shared_ptr<IArchetypeComponents>, ComponentsFilter::Hash/*,
			ComponentsFilter::LessComparator*/> archetypeComponents_;

	};
}