#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class FramesCounter : public ECSComponent<FramesCounter> {
	public:

		FramesCounter() : ECSComponent{ nullptr } { }

		Common::Size framesCount_ = 0;
	};

	template<>
	inline void Edit<FramesCounter>(FramesCounter* framesCounter) {
		ImGui::TextDisabled("Frames: %d", framesCounter->framesCount_);
	}

	class FramesCounterSystem : public ECSSystem {
	public:
		FramesCounterSystem(Context& context) : ECSSystem{ context } {

		}

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			FramesCounter* counter = world->GetComponent<FramesCounter>(entityId);
			++counter->framesCount_;
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<FramesCounter>(), ECS::Entity::Filter{}.ExcludeAll() };
		}
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "FramesCounterSystem";
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<FramesCounterSystem>().GetId();
		}


	};

}