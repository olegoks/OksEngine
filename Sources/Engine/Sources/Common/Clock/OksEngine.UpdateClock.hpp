#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {


	class UpdateClock : public ECSSystem {
	public:

		UpdateClock(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "UpdateClock";
		}
	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<UpdateClock>().GetId();
		}
	};


	//class UpdateClock {
	//public:

	//	void Update(Clock* clock) {
	//		clock->now_ = std::chrono::high_resolution_clock::now();
	//	}

	//};
}