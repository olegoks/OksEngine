
#include <OksEngine.ECS.System.hpp>
#include <Common.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.UI.Subsystem.hpp>
namespace OksEngine {

	class UISystem : public ECSSystem {
	public:

		UISystem(Context& context) : ECSSystem{ context }{}
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			auto* behaviour = world->GetComponent<Behaviour>(entityId);
			if (behaviour == nullptr) return;

			auto maybeEvent = GetContext().GetUISubsystem()->GetLastEvent();
			if (maybeEvent.has_value()) {
				std::string keyString = magic_enum::enum_name(maybeEvent.value().key_).data();
				behaviour->CallInputProcessor(keyString.c_str());
			}
		}

		virtual void EndUpdate() override {
			GetContext().GetUISubsystem()->DeleteLastEvent();
		}

		virtual Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<UISystem>().GetId();
		}

	private:
	};

}