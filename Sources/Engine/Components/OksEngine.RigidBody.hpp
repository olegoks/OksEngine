
#include <OksEngine.ECS.Component.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	struct RigidBody : public ECSComponent<RigidBody> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();
		float staticFriction_ = 0.5f;
		float dynamicFriction_ = 0.5f;
		float restitution_ = 0.5f;
		//std::shared_ptr<Geom::Box> shape_ = nullptr;
		float halfExtentX_ = 0.5f;
		float halfExtentY_ = 0.5f;
		float halfExtentZ_ = 0.5f;
		Math::Matrix4x4f transform_ = Math::Matrix4x4f::GetIdentity();
		RigidBody(
			Context* context, 
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ) :
			ECSComponent{ context },
			staticFriction_{ staticFriction },
			dynamicFriction_{ dynamicFriction },
			restitution_{ restitution },
			halfExtentX_{ halfExtentX },
			halfExtentY_{ halfExtentY },
			halfExtentZ_{ halfExtentZ } {

		}

		void SetTransform(const Math::Matrix4x4f& transform) {
			transform_ = transform;
		}

		const Math::Matrix4x4f& GetTransform() {
			return transform_;
		}

	};

}