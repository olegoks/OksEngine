
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct ResourceWasLoadedEarly : public ECSComponent<ResourceWasLoadedEarly> {
		ResourceWasLoadedEarly() :
			ECSComponent{ nullptr } {
		}
	};


	template<>
	inline void Edit<ResourceWasLoadedEarly>(ResourceWasLoadedEarly* resourceName) {
	}

}