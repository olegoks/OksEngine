
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct Camera : public ECSComponent<Camera> {
		Math::Vector3f position_ = { 5.f, 0.f, 0.f };;
		Math::Vector3f direction_ = Math::Vector3f{ Math::Vector3f{ 0.f, 0.f, 0.f } - Math::Vector3f{ 5.f, 0.f, 0.f } };
		bool isActive_ = true;
		Camera(
			Context* context,
			const Math::Vector3f& position,
			const Math::Vector3f& direction) : ECSComponent{ context } { }
		
	};

}