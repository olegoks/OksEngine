#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <PAL.RigidBody.hpp>

namespace OksEngine {

	struct StaticRigidBody : public ECSComponent<StaticRigidBody> {

		StaticRigidBody() : ECSComponent{ nullptr } { }

		StaticRigidBody(PAL::StaticRigidBody::Id id) :
			ECSComponent{ nullptr }, id_{ id } { }

		PAL::StaticRigidBody::Id id_;

	};

}