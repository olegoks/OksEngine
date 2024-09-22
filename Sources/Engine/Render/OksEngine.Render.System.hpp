
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

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<ImmutableRenderGeometry>(), ECS::Entity::Filter{}.ExcludeAll() };
		}
	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};

	class RenderImmutableRenderGeometry : public ECSSystem{
	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {



		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<ImmutableRenderGeometry>(), ECS::Entity::Filter{}.ExcludeAll() };
		}

	};



	class ResizeFrameBuffers : public ECSSystem {
	public:

		ResizeFrameBuffers(Context& context) noexcept :
			ECSSystem{ context } { }

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<ResizeFrameBuffers>().GetId();
		}

	};



}