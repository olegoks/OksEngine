#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>
#include <Physics/OksEngine.DynamicRigidBody.hpp>

namespace OksEngine {

	class DynamicRigidBodyCapsule : public DynamicRigidBody<DynamicRigidBodyCapsule> {
	public:

		DynamicRigidBodyCapsule() : DynamicRigidBody{ } {}

		DynamicRigidBodyCapsule(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			const Material& material,
			float radius,
			float height) :
			DynamicRigidBody{
				context,
				transform,
				mass,
				linearDamping,
				angularDamping,
				material
			},
			radius_{ radius },
			height_{ height } {}

		float radius_ = 0.1f;
		float height_ = 0.5f;
	};

	template<>
	inline void Edit<DynamicRigidBodyCapsule>(DynamicRigidBodyCapsule* dynamicRigidBodyCapsule) {

	}

}