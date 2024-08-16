#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.StaticRigidBody.hpp>

namespace OksEngine {

	class StaticRigidBodyCustomMeshShape : public StaticRigidBody<StaticRigidBodyCustomMeshShape> {
	public:

		StaticRigidBodyCustomMeshShape() : StaticRigidBody{ } {}

		StaticRigidBodyCustomMeshShape(
			Context* context,
			const glm::mat4& transform,
			const std::string& geomName,
			const Material& material) :
			StaticRigidBody{
				context,
				transform,
				material
			},
			geomName_{ geomName } {}
		std::string geomName_ = "";
	};



}