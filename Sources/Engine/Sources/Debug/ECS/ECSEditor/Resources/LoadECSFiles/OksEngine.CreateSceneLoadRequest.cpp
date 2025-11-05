#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles/auto_OksEngine.CreateSceneLoadRequest.hpp>

namespace OksEngine {
	void CreateSceneLoadRequest::Update(
		ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const ECSEditorWindow* eCSEditorWindow0,
		const LoadingECSFiles* loadingECSFiles0,
		ECS2::Entity::Id entity1id, const ECSController* eCSController1) {

		CreateComponent<LoadSceneRequest>(entity1id, "OksEngine.Test.scn", entity0id);
		
	};
}