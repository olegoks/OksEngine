#pragma once 

#include <Common.hpp>

#include <ECS2.Component.hpp>

namespace ECS2 {


	class EntityInfo final {
	public:

		EntityInfo() noexcept = default;

		//[[nodiscard]]
		//Common::Size GetComponentsNumber() const noexcept {
		//	return typeIndex_.size();
		//}

		//template<class ComponentType>
		//[[nodiscard]]
		//bool IsComponentExist() const noexcept {
		//	const auto it = typeIndex_.find(ComponentType::GetTypeId());
		//	return it != typeIndex_.cend();
		//}

		//template<class ComponentType>
		//[[nodiscard]]
		//ComponentIndex GetComponentIndex() const noexcept {

		//	const ComponentTypeId componentTypeId = ComponentType::GetTypeId();
		//	const ComponentIndex componentIndex = typeIndex_[componentTypeId];

		//	return componentIndex;
		//}

		template<class ComponentType>
		void AddComponent(ComponentIndex componentIndex) noexcept {

			//ComponentTypeId componentTypeId = ComponentType::GetTypeId();
			//delayedTypeIndexForAdd_.insert(std::pair{ componentTypeId, componentIndex });
			//delayedFilter_.Include<ComponentType>();
		}

		template<class ComponentType>
		void RemoveComponent() noexcept {
			//ComponentTypeId componentTypeId = ComponentType::GetTypeId();
			//delayedComponentsToRemove_.push_back(componentTypeId);
			//delayedFilter_.Exclude<ComponentType>();
		}

		//[[nodiscard]]
		//const Entity::Filter& GetFilter() const noexcept {
		//	return filter_;
		//}


		//void ProccessDelayedRequests() {

		//	filter_ = filter_ + delayedFilter_;
		//	delayedFilter_.Clear();

		//	typeIndex_.insert(
		//		delayedTypeIndexForAdd_.begin(),
		//		delayedTypeIndexForAdd_.end()
		//	);
		//	for (auto componentTypeIdToRemove : delayedComponentsToRemove_) {
		//		typeIndex_.erase(componentTypeIdToRemove);
		//	}
		//	delayedComponentsToRemove_.clear();
		//	delayedTypeIndexForAdd_.clear();
		//}

	private:
		//Entity::Filter delayedFilter_;
		//std::map<ComponentTypeId, ComponentIndex> delayedTypeIndexForAdd_;
		//std::vector<ComponentTypeId> delayedComponentsToRemove_;

		//Entity::Filter filter_; // this filter doesnt have excludes.
		//Component type to component index.
		//std::map<ComponentTypeId, ComponentIndex> typeIndex_;
	};

}