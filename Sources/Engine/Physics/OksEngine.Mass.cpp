
#include <Physics/OksEngine.Mass.hpp>

namespace OksEngine {

	Mass::Mass() : ECSComponent{ nullptr } {}

	Mass::Mass(float mass) : ECSComponent{ nullptr }, mass_{ mass } {}

}