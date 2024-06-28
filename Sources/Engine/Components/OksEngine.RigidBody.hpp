
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct RigidBody : public ECSComponent<RigidBody> {
		Common::Index id_ = Common::Limits<Common::Index>::Max();

	/*	RigidBody() {

		}*/
	};

}