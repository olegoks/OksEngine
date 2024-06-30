
#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class PhysicsSystem : public ECSSystem {
	public:

		PhysicsSystem(Context& context) : ECSSystem{ context } {

		}


		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {

		}
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<PhysicsSystem>().GetId();
		}

	private:
	};

}