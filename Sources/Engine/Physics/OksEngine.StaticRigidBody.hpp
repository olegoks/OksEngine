#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	//template<class RigidBodyType>
	//struct StaticRigidBody : public ECSComponent<RigidBodyType> {
	//	Common::Index id_ = Common::Limits<Common::Index>::Max();
	//	Material material_{ 0.5, 0.5f, 0.5f };

	//	StaticRigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

	//	StaticRigidBody(
	//		const Material& material
	//	) :
	//		ECSComponent<RigidBodyType>{ nullptr },
	//		material_{ material } { }
	//};

}