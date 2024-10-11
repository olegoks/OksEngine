
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	DynamicRigidBody::DynamicRigidBody() : ECSComponent{ nullptr } {}

	DynamicRigidBody::DynamicRigidBody(PAL::DynamicRigidBody::Id id) : ECSComponent{ nullptr }, id_{ id } {}

}