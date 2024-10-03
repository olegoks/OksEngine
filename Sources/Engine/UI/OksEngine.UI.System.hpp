
#include <OksEngine.ECS.System.hpp>
#include <Common.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>
namespace OksEngine {

	class UISystem : public ECSSystem {
	public:

		UISystem(Context& context) : ECSSystem{ context }{}
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* behaviour = world->GetComponent<Behaviour>(entityId);
			if (behaviour == nullptr) return;

			//{
			//	auto maybeKeyboardEvent = GetContext().GetUISubsystem()->GetLastKeyboardEvent();
			//	auto maybeMouseEvent = GetContext().GetUISubsystem()->GetLastMouseEvent();
			//	double offsetX = 0.0;
			//	double offsetY = 0.0;
			//	if (maybeMouseEvent.has_value()) {
			//		offsetX = maybeMouseEvent.value().GetX();
			//		offsetY = maybeMouseEvent.value().GetY();
			//	}
			//	std::string keyStr = "";
			//	std::string eventStr = "";
			//	if (maybeKeyboardEvent.has_value()) {
			//		keyStr = magic_enum::enum_name(maybeKeyboardEvent.value().key_).data();
			//		eventStr = magic_enum::enum_name(maybeKeyboardEvent.value().event_).data();

			//	}
			//	if (maybeKeyboardEvent.has_value() || maybeMouseEvent.has_value()) {
			//		behaviour->CallInputProcessor(keyStr.c_str(), eventStr.c_str(), offsetX, offsetY);
			//	}
			//}
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<Behaviour>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "UISystem";
		}

		virtual void EndUpdate() override {
			//GetContext().GetUISubsystem()->DeleteLastKeyboardEvent();
			//GetContext().GetUISubsystem()->DeleteLastMouseEvent();
		}

		virtual Common::TypeId GetTypeId() const noexcept {
			return Common::TypeInfo<UISystem>().GetId();
		}

	private:
	};

}