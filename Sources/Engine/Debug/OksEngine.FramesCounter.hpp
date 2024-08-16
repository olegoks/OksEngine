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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			FramesCounter* counter = world->GetComponent<FramesCounter>(entityId);
			++counter->framesCount_;
		}

		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<FramesCounter>();
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<FramesCounterSystem>().GetId();
		}


	};

}