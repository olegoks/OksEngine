#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.StaticRigidBody.hpp>

namespace OksEngine {

	class StaticRigidBodyCustomMeshShape : public ECSComponent<StaticRigidBodyCustomMeshShape> {
	public:
		StaticRigidBodyCustomMeshShape() : ECSComponent{ nullptr } {}
	};



}