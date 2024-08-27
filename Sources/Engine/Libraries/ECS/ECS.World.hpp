#pragma once 

#include <utility>
#include <ECS.Entity.hpp>
#include <ECS.SystemsManager.hpp>
#include <ECS.ComponentsManager.hpp>
#include <ECS.EntitiesManager.hpp>

namespace ECS {

	class [[nodiscard]] World final {
	public:

		[[nodiscard]]
		Entity::Id CreateEntity() noexcept {
			Entity::Id entityId = entitiesManager_.CreateEntity();

			return entityId;
		}

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			componentsManager_.CreateComponent<ComponentType>(entityId, std::forward<Args>(args)...);
		}

		template<class ComponentType, class ...Args>
		void CreateComponent(Entity::Filter filter, Args&&... args) noexcept {
			entitiesManager_.ForEachEntity(
				[this, &filter, &args...](Entity& entity) {
					const Entity::Filter searchFilter = filter;
					const Entity::Filter entityFilter = componentsManager_.GetEntityFilter(entity.GetId());
					if (searchFilter.Matches(entityFilter)) {
						componentsManager_.CreateComponent<ComponentType>(entity.GetId(), std::forward<Args>(args)...);
					}
				});
			
		}

		template<class ComponentType>
		void RemoveComponent(Entity::Id entityId) noexcept {
			componentsManager_.RemoveComponent<ComponentType>(entityId);
		}
		template<class SystemType, class ...Args>
		std::shared_ptr<SystemType> RegisterSystem(Args&& ...args) noexcept {
			return systemsManager_.RegisterSystem<SystemType>(std::forward<Args>(args)...);
		}


		template<class ComponentType>
		[[nodiscard]]
		ComponentType* GetComponent(Entity::Id entityId) noexcept {
			return componentsManager_.GetComponent<ComponentType>(entityId);
		}

		template<class ComponentType>
		[[nodiscard]]
		bool IsComponentExist(Entity::Id entityId) noexcept {
			return componentsManager_.IsComponentExist<ComponentType>(entityId);
		}

		void Process() noexcept {
			/*entitiesManager_.ForEachEntity(
				[this](Entity::Id entityId) {
					systemsManager_.ForEachSystem(
						[entityId, this](std::shared_ptr<System> system)->bool {
							system->StartUpdate();
							return true;
						});
				});
			entitiesManager_.ForEachEntity(
				[this](Entity::Id entityId) {
					systemsManager_.ForEachSystem(
						[entityId, this](std::shared_ptr<System> system)->bool {
							system->Update(this, entityId);
							return true;
						});
				});
			entitiesManager_.ForEachEntity(
				[this](Entity::Id entityId) {
					systemsManager_.ForEachSystem(
						[entityId, this](std::shared_ptr<System> system)->bool {
							system->EndUpdate();
							return true;
						});
				});*/
		}

		void StartFrame() {
			componentsManager_.AddDelayedComponents();
		}

		void EndFrame() {
			
			componentsManager_.AddDelayedComponents();
		}

		template<class SystemType>
		void RunSystem() {

			std::shared_ptr<System> system = systemsManager_.GetSystem<SystemType>();
			OS::AssertMessage(system != nullptr, "Error while Getting system. Maybe you dont register this system in the ecs world.");
			system->BeforeUpdate(this);
			if (system->GetFilter().first != Entity::Filter().ExcludeAll()) {
				if (system->GetFilter().second == Entity::Filter().ExcludeAll()) {
					// One entity system.
					entitiesManager_.ForEachEntity(
						[this, &system](Entity& entity) {
							const auto systemFilter = system->GetFilter();
							const Entity::Filter entityFilter = componentsManager_.GetEntityFilter(entity.GetId());
							if (systemFilter.first.Matches(entityFilter)) {
								system->Update(this, entity.GetId(), Entity::Id::invalid_);
							}
						});
				} else {
					// One entity system.
					entitiesManager_.ForEachEntity(
						[this, &system](Entity& firstEntity) {
							const auto systemFilter = system->GetFilter();
							const Entity::Filter firstEntityFilter = componentsManager_.GetEntityFilter(firstEntity.GetId());
							if (systemFilter.first.Matches(firstEntityFilter)) {
								entitiesManager_.ForEachEntity(
									[this, &system, firstEntity](Entity& secondEntity) {
										const auto systemFilter = system->GetFilter();
										const Entity::Filter secondEntityFilter = componentsManager_.GetEntityFilter(secondEntity.GetId());
										if (systemFilter.second.Matches(secondEntityFilter)) {
											system->Update(this, firstEntity.GetId(), secondEntity.GetId());
										}
									});
							}

						});
				}
			}

			system->AfterUpdate(this);
		}

		[[nodiscard]]
		Common::Size GetEntitiesNumber() const noexcept {
			return entitiesManager_.GetEntitiesNumber();
		}

		//[[nodiscard]]
		//std::vector<Entity::Id> GetEntitiesIds() noexcept {
		//	std::vector<Entity::Id> entitiesIds;
		//	entitiesIds.reserve(entitiesManager_.GetEntitiesNumber());
		//	entitiesManager_.ForEachEntity(
		//		[&entitiesIds](Entity& entity) {
		//			entitiesIds.push_back(entity.GetId());
		//		});
		//	return entitiesIds;
		//}

		[[nodiscard]]
		Entity::Filter GetEntityFilter(Entity::Id entityId) {
			return entitiesManager_.GetEntityFilter(entityId);
		}

		[[nodiscard]]
		World& operator=(const World& copyWorld) const noexcept {
			OS::NotImplemented();
		}

	private:

		SystemsManager systemsManager_;
		EntitiesManager entitiesManager_;
		ComponentsManager componentsManager_;
	};
}