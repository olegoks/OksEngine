#pragma once 

#include <ECS2.Entity.hpp>

namespace ECS2 {

	class Archetype {
	public:

		Archetype(const ComponentsFilter& include) {

		}

		[[nodiscard]]
		const ComponentsFilter& GetComponentsFilter() {
			return include_;
		}

		using ProcessComponent = std::function<void(ComponentTypeId componentTypeId)>;


		void ForEachComponent(ProcessComponent&& processComponent) const {
			for (Common::UInt16 componentTypeId = 0;
				componentTypeId < include_.GetMaxComponentsNumber();
				componentTypeId++) {
				if (include_.IsSet(componentTypeId)) {
					processComponent(componentTypeId);
				}
			}
		}


		ComponentsFilter include_;
	};


}