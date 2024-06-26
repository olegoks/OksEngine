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

		template<class SystemType, class ...Args>
		std::shared_ptr<SystemType> RegisterSystem(Args&& ...args) noexcept {
			return systemsManager_.RegisterSystem<SystemType>(std::forward<Args>(args)...);
		}


		template<class ComponentType>
		[[nodiscard]]
		ComponentType* GetComponent(Entity::Id entityId) noexcept {
			return componentsManager_.GetComponent<ComponentType>(entityId);
		}

		void Process() noexcept {
			entitiesManager_.ForEachEntity(
				[this](Entity::Id entityId) {
					systemsManager_.ForEachSystem(
						[entityId, this](std::shared_ptr<System> system)->bool {
							system->Update(this, entityId);
							return true;
						});
				});
		}

		template<class SystemType>
		void RunSystem() {
			entitiesManager_.ForEachEntity(
				[this](Entity::Id entityId) {
					std::shared_ptr<System> system = systemsManager_.GetSystem<SystemType>();
					system->Update(this, entityId);
				});
		}

		[[nodiscard]]
		Common::Size GetEntitiesNumber() const noexcept {
			return entitiesManager_.GetEntitiesNumber();
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