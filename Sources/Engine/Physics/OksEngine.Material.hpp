#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	class Material : public ECSComponent<Material> {
	public:

		Material();
		Material(float staticFriction, float dynamicFriction, float restitution);

		float staticFriction_;
		float dynamicFriction_;
		float restitution_;
	};

}