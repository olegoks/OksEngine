#pragma once 

#include <OksEngine.ECS.System.hpp>

#include <Common/FramesCounter/OksEngine.FramesCounter.hpp>
#include <Common/auto_OksEngine.Counter.hpp>

namespace OksEngine {

	class IncreaseFramesCounter : public ECSSystem {
	public:
		IncreaseFramesCounter(Context& context) : ECSSystem{ context } {

		}

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* counter = world->GetComponent<Counter>(entityId);
			++counter->value_;
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<FramesCounter>().Include<Counter>(), ECS::Entity::Filter{}.ExcludeAll() };
			return filter;
		}
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "FramesCounterSystem";
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<IncreaseFramesCounter>().GetId();
		}


	};

}