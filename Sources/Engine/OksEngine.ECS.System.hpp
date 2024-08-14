#pragma once 

#include <ECS.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {

	class ECSSystem : public ECS::System {
	public:
		ECSSystem(Context& context) : 
			context_{ context } { }

		[[nodiscard]]
		Context& GetContext() noexcept {
			return context_;
		}

		virtual void StartUpdate() override { }
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {

		}
		virtual void EndUpdate() override { }

		//virtual void NewUpdate(ECS::Accessor::Entity& entity) override {

		//}

		virtual ECS::Entity::Filter GetFilter() const noexcept override { 
			return ECS::Entity::Filter{}.ExcludeAll();
		}

	private:
		Context& context_;
	};

}