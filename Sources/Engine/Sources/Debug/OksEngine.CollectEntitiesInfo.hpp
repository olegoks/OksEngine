#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {


	class CollectEntitiesInfo : public ECSSystem {
	public:

		CollectEntitiesInfo(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void BeforeUpdate(ECS::World* world) override;
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		void ShowEntityInfo(ECS::World* world, ECS::Entity::Id id);

		virtual void AfterUpdate(ECS::World* world) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}, ECS::Entity::Filter{}.ExcludeAll() };
			return filter;
		}
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "CollectEntitiesInfo";
		}
	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CollectEntitiesInfo>().GetId();
		}
	private:

		struct EntityState {
			int currentAddComponentIndex_ = 0;
		};

		[[nodiscard]]
		EntityState& GetCreateState(ECS::Entity::Id id) {

			auto it = states_.find(id);
			if (it == states_.end()) {
				states_.insert({ id, EntityState{ 0 } });
			}
			return states_[id];
		}

		std::map<ECS::Entity::Id, EntityState> states_;
	};

}