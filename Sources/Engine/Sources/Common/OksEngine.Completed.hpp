#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Completed : public ECSComponent<Completed> {
	public:

		Completed() : ECSComponent{ nullptr } {}
	};


	template<>
	inline void Edit<Completed>(Completed* completed) {

	}

}