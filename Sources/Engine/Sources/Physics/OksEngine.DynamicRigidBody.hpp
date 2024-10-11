#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <PAL.RigidBody.hpp>

namespace OksEngine {


	class DynamicRigidBody : public ECSComponent<DynamicRigidBody> {
	public:

		DynamicRigidBody();

		DynamicRigidBody(PAL::DynamicRigidBody::Id id);

		PAL::DynamicRigidBody::Id id_;
	};

}