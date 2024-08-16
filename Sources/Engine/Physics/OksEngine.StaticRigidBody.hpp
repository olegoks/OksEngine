#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	template<class RigidBodyType>
	struct StaticRigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		Material material_{ 0.5, 0.5f, 0.5f };

		StaticRigidBody() : ECSComponent<RigidBodyType>{ nullptr } { }

		StaticRigidBody(
			Context* context,
			const glm::mat4& transform,
			const Material& material
		) :
			ECSComponent<RigidBodyType>{ context },
			transform_{ transform },
			material_{ material } { }

		const glm::mat4& GetTransform() {
			return transform_;
		}

	};

}