#include <Debug\auto_OksEngine.CreateECSInspector.hpp>

namespace OksEngine {

	void CreateECSInspector::Update() {

		ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<ImGuiState>(entityId);
		CreateComponent<HandleKeyboardInput>(entityId);
		//CreateComponent<MainMenuBar>(entityId);
		//CreateComponent<EnginePerformance>(entityId);
		//CreateComponent<ECSInspector>(entityId);
		//CreateComponent<FramesCounter>(entityId);

	};
}