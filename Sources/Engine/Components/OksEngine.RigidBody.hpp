
#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	template<class RigidBodyType>
	struct RigidBody : public ECSComponent<RigidBodyType> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		glm::mat4 transform_ = glm::identity<glm::mat4>();
		float mass_ = 10.f;
		float staticFriction_ = 0.5f;
		float dynamicFriction_ = 0.5f;
		float restitution_ = 0.5f;

		RigidBody() : ECSComponent<RigidBodyType>{ nullptr }
		{
			
		}
		RigidBody(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float staticFriction,
			float dynamicFriction,
			float restitution) :
			ECSComponent<RigidBodyType>{ context },
			transform_{ transform },
			mass_{ mass },
			staticFriction_{ staticFriction },
			dynamicFriction_{ dynamicFriction },
			restitution_{ restitution } {	}

		void SetTransform(const glm::mat4& transform) {
			transform_ = transform;
		}

		const glm::mat4& GetTransform() {
			return transform_;
		}

	};


	class RigidBodyBox: public RigidBody<RigidBodyBox> {
	public:

		RigidBodyBox() : RigidBody{ } {}

		RigidBodyBox(
			Context* context,
			const glm::mat4& transform,
			float mass,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ) : 
			RigidBody{ 
				context, 
				transform,
				mass,
				staticFriction,
				dynamicFriction,
				restitution
			},
			halfExtentX_{ halfExtentX },
			halfExtentY_{ halfExtentY },
			halfExtentZ_{ halfExtentZ } {}

		float halfExtentX_ = 0.5f;
		float halfExtentY_ = 0.5f;
		float halfExtentZ_ = 0.5f;
	};

}