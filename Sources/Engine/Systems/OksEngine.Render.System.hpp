
#pragma once

#include <OksEngine.Context.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>
#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class RenderSystem : public ECSSystem {
	public:

		RenderSystem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}