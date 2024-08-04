
#include <Debug/OksEngine.MainMenuBar.hpp>

namespace OksEngine {

	MainMenuBar::MainMenuBar() : ECSComponent{ nullptr } {

	}

	MainMenuBar::MainMenuBar(
		Context* context) :
		ImGuiWindow{ "Main menu bar", false },
		ECSComponent{ context } {}

	EnginePerformance::EnginePerformance() : ECSComponent{ nullptr } {

	}

	EnginePerformance::EnginePerformance(
		Context* context) :
		ImGuiWindow{ "Engine Performance", false },
		ECSComponent{ context } {}


	ECSInspector::ECSInspector() : ECSComponent{ nullptr } {

	}

	ECSInspector::ECSInspector(
		Context* context) :
		ImGuiWindow{ "Engine Performance", false },
		ECSComponent{ context } {}


}