
#pragma once


#include <OksEngine.Context.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Debug/OksEngine.MainMenuBar.hpp>

namespace OksEngine {

	class MainMenuBarSystem : public ECSSystem {
	public:

		MainMenuBarSystem(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<MainMenuBarSystem>().GetId();
		}

	};


	class EnginePerformanceSystem : public ECSSystem {
	public:

		EnginePerformanceSystem(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<EnginePerformanceSystem>().GetId();
		}

	};


	class ECSInspectorSystem : public ECSSystem {
	public:

		ECSInspectorSystem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}