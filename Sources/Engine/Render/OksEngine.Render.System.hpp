
#pragma once

#include <OksEngine.Context.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/OksEngine.SkinnedGeometry.hpp>
#include <Render/OksEngine.Camera.hpp>
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


	class CameraSystem : public ECSSystem {
	public:

		CameraSystem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};


}