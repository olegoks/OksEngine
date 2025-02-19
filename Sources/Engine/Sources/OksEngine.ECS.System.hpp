#pragma once 

#include <ECS.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {

	/*template<class SystemType = int>*/
	class ECSSystem : public ECS::System {
	public:
		ECSSystem(Context& context) : 
			context_{ context } { }

		[[nodiscard]]
		Context& GetContext() noexcept {
			return context_;
		}
		virtual void BeforeUpdate(ECS::World* world) override {}
		virtual void StartUpdate() override { }
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override { }
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override { }
		virtual void EndUpdate() override { }
		virtual void AfterUpdate(ECS::World* world) override {}

		virtual std::string GetName() const noexcept override {
			return "No name";
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.ExcludeAll(), ECS::Entity::Filter{}.ExcludeAll() };
		}

	private:
		Context& context_;
	};

	class ECSSystem2 : public ECS::System {
	public:
		ECSSystem2() { }
		virtual void BeforeUpdate(ECS::World* world) override {}
		virtual void StartUpdate() override { }
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override { }
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override { }
		virtual void EndUpdate() override { }
		virtual void AfterUpdate(ECS::World* world) override {}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.ExcludeAll(), ECS::Entity::Filter{}.ExcludeAll() };
		}
	};

}