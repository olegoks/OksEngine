#include <UI/auto_OksEngine.ProcessInput.hpp>


namespace OksEngine {


	void ProcessInput::Update(
		ECS2::Entity::Id entityId,
		MainWindow* mainWindow) {

		mainWindow->window_->ProcessInput();
	}



}