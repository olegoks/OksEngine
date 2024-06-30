
#include <OksEngine.ECS.System.hpp>
#include <OksEngine.Behaviour.hpp>
namespace OksEngine {

	class UISystem : public ECSSystem {
	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			auto* behaviour = world->GetComponent<Behaviour>(entityId);
			if (behaviour == nullptr) return;

		}

		virtual Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<UISystem>().GetId();
		}

	private:
	};

}