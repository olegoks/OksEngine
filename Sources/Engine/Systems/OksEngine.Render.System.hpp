
#pragma once

#include <ECS.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.RenderableGeometry.hpp>

namespace OksEngine {

	class RenderSystem : public ECS::System {
	public:

		RenderSystem(Context& context) noexcept :
			context_{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) const override {
			auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (immutableRenderGeometry == nullptr) return;
			//context_.GetRenderSubsystem()->

		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderSystem>().GetId();
		}

		Context& context_;
	};

}