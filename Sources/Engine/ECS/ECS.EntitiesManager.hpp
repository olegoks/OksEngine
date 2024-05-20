#pragma once

#include <unordered_set>
#include <functional>
#include <cstdint>
#include <map>

#include <ECS.Entity.hpp>

namespace ECS {


	class EntitiesManager final {
	public:

		EntitiesManager() noexcept { }

		Entity::Id CreateEntity() noexcept {
			Maybe<Entity::Id> freeId = GetFreeId();
			if (freeId.has_value()) {

				idEntity_.insert(std::pair<Entity::Id, Entity*>{ freeId.value(), new Entity{} });
				return freeId.value();
			}
			return Entity::Id::invalid_;
		}

		using ProcessEntity = std::function<void(Entity::Id)>;
		void ForEachEntity(ProcessEntity&& processEntity) noexcept {
			for (auto& [idEntity, entity] : idEntity_) {
				processEntity(idEntity);
			 }
		}

		[[nodiscard]]
		Common::Size GetEntitiesNumber() const noexcept {
			return idEntity_.size();
		}

	private:
		[[nodiscard]]
		Maybe<Entity::Id> GetFreeId() noexcept {
			for (
				Entity::Id entityId = 0;
				entityId < 1000;
				entityId++) {
				const bool isIdFree = !IsEntityExist(entityId);
				if (isIdFree) {
					return entityId;
				}

			}
			return {};
		}

		[[nodiscard]]
		Entity* GetEntity(Entity::Id entityId) noexcept {
			OS::AssertMessage(IsEntityExist(entityId), "Attempt to get entity by id, but entity doesn't exist.");
			auto it = idEntity_.find(entityId);
			return it->second;
		}

		[[nodiscard]]
		bool IsEntityExist(Entity::Id entityId) const noexcept {
			const auto it = idEntity_.find(entityId);
			return (it != idEntity_.end());
		}

		std::map<Entity::Id, Entity*> idEntity_;
	};

}