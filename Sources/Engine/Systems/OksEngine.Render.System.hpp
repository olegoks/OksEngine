
#pragma once

#include <ECS.hpp>
#include <OksEngine.Context.hpp>
#include <OksEngine.RenderableGeometry.hpp>

namespace OksEngine {

	class RenderSystem : public ECS::System {
	public:

		RenderSystem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) const override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;

		Context& context_;
	};

}