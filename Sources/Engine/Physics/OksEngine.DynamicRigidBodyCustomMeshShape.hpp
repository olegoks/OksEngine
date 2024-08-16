#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	class DynamicRigidBodyCustomMeshShape : public DynamicRigidBody<DynamicRigidBodyCustomMeshShape> {
	public:

		DynamicRigidBodyCustomMeshShape() : DynamicRigidBody{ } {}

		DynamicRigidBodyCustomMeshShape(
			Context* context,
			const glm::mat4& transform,
			const std::string& geomName,
			float mass,
			float linearDamping,
			float angularDamping,
			const Material& material) :
			DynamicRigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				material
			}, meshName_{ geomName } {}


		std::string meshName_;
	};

}