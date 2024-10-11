
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct WaitingForResourceLoading : public ECSComponent<WaitingForResourceLoading> {
		WaitingForResourceLoading() :
			ECSComponent{ nullptr } {
		}
	};


	template<>
	inline void Edit<WaitingForResourceLoading>(WaitingForResourceLoading* resourceName) {
	}

}