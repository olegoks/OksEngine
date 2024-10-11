#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Mass : public ECSComponent<Mass> {
	public:

		Mass();

		Mass(float mass);

		float mass_ = 10.f;
	};

}