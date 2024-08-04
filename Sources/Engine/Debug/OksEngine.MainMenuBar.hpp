#pragma once 

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct ImGuiWindow {
		std::string name_ = "";
		bool show_ = false;
	};

	struct MainMenuBar : public ImGuiWindow, public ECSComponent<MainMenuBar> {
	public:
		std::vector<std::string> items_{
			"Engine Performance",
			"ECS Inspector"		
		};
		MainMenuBar();

		MainMenuBar(Context* context);
	public:
		bool showPerformanceProfiler_ = false;
		bool showECSInspector_ = false;
	};

	struct EnginePerformance : public ImGuiWindow, public ECSComponent<EnginePerformance> {
	public:
		EnginePerformance();

		EnginePerformance(Context* context);

	};

	struct ECSInspector : public ImGuiWindow, public ECSComponent<ECSInspector> {
	public:
		ECSInspector();

		ECSInspector(Context* context);

		struct EntityState {
			int currentAddComponentIndex_ = 0;
			char addComponentName_[64]{ "No component" };
			bool addComponent_ = false;
		};

		[[nodiscard]]
		EntityState& GetCreateState(ECS::Entity::Id id) {

			auto it = states_.find(id);
			if (it == states_.end()) {
				states_.insert({ id, ECSInspector::EntityState{ 0 } });
			}
			return states_[id];
		}

		std::map<ECS::Entity::Id, EntityState> states_;

	};

}