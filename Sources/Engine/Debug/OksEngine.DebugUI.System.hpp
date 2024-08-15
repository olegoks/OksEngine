
#pragma once


#include <OksEngine.Context.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Debug/OksEngine.MainMenuBar.hpp>
#include <Common/OksEngine.FramesCounter.hpp>

namespace OksEngine {


	class ImGuiSystem : public ECSSystem {
	public:

		ImGuiSystem(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:
		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<ImGuiContext>();
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<ImGuiSystem>().GetId();
		}

	};

	class MainMenuBarSystem : public ECSSystem {
	public:

		MainMenuBarSystem(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:
		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<MainMenuBar>();
		}
		
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<MainMenuBarSystem>().GetId();
		}

	};


	class EnginePerformanceSystem : public ECSSystem {
	public:

		EnginePerformanceSystem(Context& context) : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;
		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<EnginePerformance>().Include<FramesCounter>();
		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<EnginePerformanceSystem>().GetId();
		}

	};


	class CollectEntitiesInfo : public ECSSystem {
	public:

		CollectEntitiesInfo(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void BeforeUpdate(ECS::World* world) override;
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

		virtual void AfterUpdate(ECS::World* world) override;

		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{};
		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CollectEntitiesInfo>().GetId();
		}
	private:

		struct EntityState {
			int currentAddComponentIndex_ = 0;
			//char addComponentName_[64]{ "No component" };
			//bool addComponent_ = false;
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

	class ECSInspectorSystem : public ECSSystem {
	public:

		ECSInspectorSystem(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{}.Include<ECSInspector>();
		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}