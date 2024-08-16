#pragma once 

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct ImGuiContext : public ECSComponent<ImGuiContext> {
	public:
		ImGuiContext() : ECSComponent{ nullptr } {}
	};

	struct MainMenuBar : public ECSComponent<MainMenuBar> {
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


	template<>
	inline void Edit<MainMenuBar>(MainMenuBar* mainMenuBar) {

	}

}