
#include <Debug/OksEngine.MainMenuBar.hpp>

namespace OksEngine {

	MainMenuBar::MainMenuBar() : ECSComponent{ nullptr } {

	}

	MainMenuBar::MainMenuBar(
		Context* context) :
		ECSComponent{ context } {}

}