#pragma once 

#include <utility>
#include <memory>
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

		Entity::Id GetFreeEntityId() {
			auto freeIdIt = freeEntityIds_.begin();
			const Entity::Id freeId = *freeIdIt;
			freeEntityIds_.erase(freeIdIt);
			return freeId;
		}

		//Create archetype entity.
		template<class ...Components>
		[[nodiscard]]
		Entity::Id CreateEntity() noexcept {
			const Entity::Id entityId = GetFreeEntityId();

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
			return entityId;
		}

		Entity::Id CreateEntity() noexcept {
			const Entity::Id entityId = GetFreeEntityId();
			dynamicEntitiesComponentFilters_[entityId] = ComponentsFilter{};
			return entityId;
		}

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {

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
				archetypeComponents->CreateComponent<ComponentType>(entityId, std::forward<Args>(args)...);
			}
			else {
				auto container = std::dynamic_pointer_cast<Container<ComponentType>>(dynamicEntitiesContainers_[ComponentType::GetTypeId()]);
				if (container == nullptr) {
					auto newContainer = std::make_shared<Container<ComponentType>>();
					dynamicEntitiesContainers_[ComponentType::GetTypeId()] = newContainer;
					container = newContainer;
				}
				container->CreateComponent(entityId, std::forward<Args>(args)...);
				dynamicEntitiesComponentFilters_[entityId].SetBits<ComponentType>();
			}
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId) noexcept {

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
				container->RemoveComponent<ComponentType>();
				dynamicEntitiesComponentFilters_[entityId].RemoveBits<ComponentType>();
			}

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
				return container->GetComponent<ComponentType>(entityId);
			}
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) noexcept {
			return false;
		}

		template<class ...Components>
		using SystemUpdateFunciton = std::function<void(Components*...)>;

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

		template<class ...Components>
		void ForEachEntity(const ComponentsFilter& exclude, SystemUpdateFunciton<Components...>&& processEntity) {
			ComponentsFilter componentsFilter;
			componentsFilter.SetBits<Components...>();

			std::shared_ptr<IArchetypeComponents> archetypeComponents = archetypeComponents_[componentsFilter];
			if (archetypeComponents) {
				//TODO: Add exclude.
				archetypeComponents->ForEachEntity<Components...>(/*exclude,*/ std::forward<SystemUpdateFunciton<Components...>&&>(processEntity));
			}
			{
				auto containers = std::make_tuple(std::dynamic_pointer_cast<Container<Components>>(dynamicEntitiesContainers_[Components::GetTypeId()])...);
				auto firstContainer = std::get<0>(containers);
				if constexpr (sizeof...(Components) == 1) {
					firstContainer->ForEachComponent(
						[&](FirstT<Components...>* component, Entity::Id entityId) {
							processEntity(component);
						});
				}
				else {
					auto containersExcludeFirst = make_tuple_excluding_first(std::get<std::shared_ptr<Container<Components>>>(containers)...);
					firstContainer->ForEachComponent(
						[&](FirstT<Components...>* component, Entity::Id entityId) {
							std::apply([&](auto&&... containers) {

								processEntity(component, containers->GetComponent(entityId)...);
								}, containersExcludeFirst);

						});
				}
			}


		}

		[[nodiscard]]
		bool IsEntityExist(Entity::Id entityId) const noexcept {
			return
				dynamicEntitiesComponentFilters_.contains(entityId)
				|| archetypeEntitiesComponents_.contains(entityId);
		}

	private:
		std::set<Entity::Id, std::less<Entity::Id>> freeEntityIds_;

		std::map<Entity::Id, ComponentsFilter> dynamicEntitiesComponentFilters_; //Components filter contains current entity components.
		std::map<ComponentTypeId, std::shared_ptr<IContainer>> dynamicEntitiesContainers_;


		std::map<Entity::Id, ComponentsFilter> archetypeEntitiesComponents_; // Components filter contains archetype components.
		std::map<
			ComponentsFilter,
			std::shared_ptr<IArchetypeComponents>,
			ComponentsFilter::LessComparator> archetypeComponents_;

	};
}