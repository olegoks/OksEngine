#pragma once

#include <unordered_set>
#include <functional>
#include <cstdint>
#include <map>

#include <ECS2.Entity.hpp>
#include <ECS2.ComponentsFilter.hpp>

namespace ECS2 {


	class EntitiesManager final {
	public:

		EntitiesManager() noexcept { }

		//[[nodiscard]]
		//Entity::Id CreateEntity() noexcept {
		//	Maybe<Entity::Id> freeId = GetFreeId();
		//	ASSERT_FMSG(freeId.has_value(), "Error while creating entity.");

		//	idEntity_.insert(std::pair{ freeId.value(), std::make_unique<Entity>(freeId.value()) });
		//	return freeId.value();
		//}

		//[[nodiscard]]
		//Entity::Id CreateEntity(const ComponentsFilter& archetype) noexcept {
		//	Maybe<Entity::Id> freeId = GetFreeId();
		//	ASSERT_FMSG(freeId.has_value(), "Error while creating entity.");
		//	idEntity_.insert(std::pair{ freeId.value(), std::make_unique<Entity>(freeId.value(), archetype) });
		//	return freeId.value();
		//}

	//	void DestroyEntity(Entity::Id id) noexcept {
	//		ASSERT_FMSG(
	//			idEntity_.contains(id),
	//			"Error while deleting entity.");
	//		idEntity_.erase(id);
	//	}

	//	using ProcessEntity = std::function<void(Entity&)>;
	//	void ForEachEntity(ProcessEntity&& processEntity) noexcept {
	//		for (auto& [idEntity, entity] : idEntity_) {
	//			processEntity(*entity);
	//		}
	//	}

	//	//[[nodiscard]]
	//	//Entity::Filter& GetEntityFilter(Entity::Id id) noexcept {
	//	//	return GetEntity(id).GetFilter();
	//	//}

	//	[[nodiscard]]
	//	Common::Size GetEntitiesNumber() const noexcept {
	//		return idEntity_.size();
	//	}

	private:/*
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
			ASSERT_FAIL_MSG("There are no free id's.");
			return {};
		}*/

	//	[[nodiscard]]
	//	Entity& GetEntity(Entity::Id entityId) noexcept {
	//		ASSERT_FMSG(IsEntityExist(entityId), "Attempt to get entity by id, but entity doesn't exist.");
	//		auto it = idEntity_.find(entityId);
	//		return *it->second;
	//	}

		//[[nodiscard]]
		//bool IsEntityExist(Entity::Id entityId) const noexcept {
		//	const auto it = idEntity_.find(entityId);
		//	return (it != idEntity_.end());
		//}

		std::map<Entity::Id, ComponentsFilter> entityComponents_;
		//std::map<Entity::Id, std::unique_ptr<Entity>> idEntity_;
	};

}