
#include <Physics/OksEngine.Material.hpp>

namespace OksEngine {

	Material::Material() : ECSComponent{ nullptr } {}

	Material::Material(float staticFriction, float dynamicFriction, float restitution) : ECSComponent{ nullptr },
		staticFriction_{ staticFriction },
		dynamicFriction_{ dynamicFriction },
		restitution_{ restitution } {}

}